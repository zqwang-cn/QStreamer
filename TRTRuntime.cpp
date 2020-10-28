#include "TRTRuntime.h"
#include <assert.h>
#include <numeric>
#include <fstream>
#include <iostream>
#include <cuda_runtime_api.h>
#include <NvOnnxParser.h>
#include <NvCaffeParser.h>

class Logger : public nvinfer1::ILogger
{
    void log(nvinfer1::ILogger::Severity severity, const char *msg) override
    {
        // suppress info-level messages
        if (severity <= nvinfer1::ILogger::Severity::kWARNING)
            std::cout << msg << std::endl;
    }
} gLogger;

inline unsigned int get_element_num(const nvinfer1::Dims &d)
{
    return std::accumulate(d.d, d.d + d.nbDims, 1, std::multiplies<int>());
}

inline unsigned int get_element_size(const nvinfer1::DataType &t)
{
    switch (t)
    {
    case nvinfer1::DataType::kINT32:
        return 4;
    case nvinfer1::DataType::kFLOAT:
        return 4;
    case nvinfer1::DataType::kHALF:
        return 2;
    case nvinfer1::DataType::kINT8:
        return 1;
    }
    return 0;
}

nvinfer1::ICudaEngine *load_engine(TRTModelType type, std::string model_file, std::string proto_file, std::vector<std::string> outputs)
{
    nvinfer1::ICudaEngine *engine = nullptr;
    std::string engine_file = model_file + ".engine";
    std::ifstream f(engine_file, std::ios::in | std::ios::binary);
    if (f)
    {
        // if engine file exists, read it
        size_t count;
        f.read((char *)&count, sizeof(count));
        char *buf = new char[count];
        f.read(buf, count);
        f.close();
        nvinfer1::IRuntime *runtime = nvinfer1::createInferRuntime(gLogger);
        engine = runtime->deserializeCudaEngine(buf, count, nullptr);
        runtime->destroy();
        delete[] buf;
    }
    else
    {
        // if not, parse from model file
        nvinfer1::IBuilder *builder = nvinfer1::createInferBuilder(gLogger);
        const auto explicitBatch = 1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
        nvinfer1::INetworkDefinition *network = builder->createNetworkV2(explicitBatch);
        nvcaffeparser1::ICaffeParser *caffe_parser;
        nvonnxparser::IParser *onnx_parser;
        switch (type)
        {
        case TRTModelType::CAFFE:
        {
            caffe_parser = nvcaffeparser1::createCaffeParser();
            const nvcaffeparser1::IBlobNameToTensor *blobNameToTensor = caffe_parser->parse(proto_file.c_str(), model_file.c_str(), *network, nvinfer1::DataType::kFLOAT);
            for (std::string s : outputs)
                network->markOutput(*blobNameToTensor->find(s.c_str()));
            break;
        }
        case TRTModelType::ONNX:
        {
            onnx_parser = nvonnxparser::createParser(*network, gLogger);
            onnx_parser->parseFromFile(model_file.c_str(), (int)nvinfer1::ILogger::Severity::kWARNING);
            break;
        }
        case TRTModelType::UFF:
        {
            // TODO
            break;
        }
        }
        //builder->setMaxBatchSize(1);
        nvinfer1::IBuilderConfig *config = builder->createBuilderConfig();
        config->setMaxWorkspaceSize(1 << 20);
        engine = builder->buildEngineWithConfig(*network, *config);
        config->destroy();
        switch (type)
        {
        case TRTModelType::CAFFE:
            caffe_parser->destroy();
            break;
        case TRTModelType::ONNX:
            onnx_parser->destroy();
            break;
        case TRTModelType::UFF:
            // TODO
            break;
        }
        network->destroy();
        builder->destroy();

        // save engine file for next time use
        nvinfer1::IHostMemory *serialized_engine = engine->serialize();
        size_t count = serialized_engine->size();
        std::ofstream f(engine_file, std::ios::out | std::ios::binary);
        f.write((const char *)&count, sizeof(count));
        f.write((const char *)serialized_engine->data(), count);
        f.close();
        serialized_engine->destroy();
    }
    return engine;
}

TRTRuntime::TRTRuntime(TRTModelType type, std::string model_file, std::string proto_file, const std::vector<std::string> outputs)
{
    // load engine and create context
    engine = load_engine(type, model_file, proto_file, outputs);
    context = engine->createExecutionContext();
    n_bindings = engine->getNbBindings();
    // alloc buffers
    buf_size = new unsigned int[n_bindings];
    cpu_buf = new void *[n_bindings];
    gpu_buf = new void *[n_bindings];
    is_input = new bool[n_bindings];
    for (int i = 0; i < n_bindings; i++)
    {
        unsigned int element_num = get_element_num(engine->getBindingDimensions(i));
        unsigned int element_size = get_element_size(engine->getBindingDataType(i));
        buf_size[i] = element_num * element_size;
        cpu_buf[i] = malloc(buf_size[i]);
        cudaMalloc(&gpu_buf[i], buf_size[i]);
        is_input[i] = engine->bindingIsInput(i);
    }
}

TRTRuntime::~TRTRuntime()
{
    // release buffers
    for (int i = 0; i < n_bindings; i++)
    {
        free(cpu_buf[i]);
        cudaFree(gpu_buf[i]);
    }
    delete[] buf_size;
    delete[] cpu_buf;
    delete[] gpu_buf;
    delete[] is_input;
    // destroy context and engine
    context->destroy();
    engine->destroy();
}

bool TRTRuntime::execute()
{
    // copy inputs to gpu
    for (int i = 0; i < n_bindings; i++)
        if (is_input[i])
            cudaMemcpy(gpu_buf[i], cpu_buf[i], buf_size[i], cudaMemcpyHostToDevice);
    // execute
    bool status = context->execute(1, gpu_buf);
    // copy outputs to cpu
    for (int i = 0; i < n_bindings; i++)
        if (!is_input[i])
            cudaMemcpy(cpu_buf[i], gpu_buf[i], buf_size[i], cudaMemcpyDeviceToHost);
    return status;
}

void *TRTRuntime::get_buf(int i)
{
    assert(i >= 0 && i < n_bindings);
    return cpu_buf[i];
}

unsigned int TRTRuntime::get_buf_size(int i)
{
    assert(i >= 0 && i < n_bindings);
    return buf_size[i];
}

nvinfer1::ICudaEngine *TRTRuntime::get_engine()
{
    return engine;
}