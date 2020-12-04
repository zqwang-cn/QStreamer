#pragma once
#include <string>
#include <vector>
#include <NvInfer.h>

enum class TRTModelType
{
	CAFFE,
	ONNX,
	UFF
};

class TRTRuntime
{
public:
	TRTRuntime(TRTModelType type, std::string model_file, std::string proto_file = "", const std::vector<std::string> outputs = std::vector<std::string>());
	~TRTRuntime();
	bool execute();
	void *get_buf(int i);
	unsigned int get_buf_size(int i);
	nvinfer1::ICudaEngine *get_engine();

private:
	nvinfer1::ICudaEngine *engine;
	nvinfer1::IExecutionContext *context;
	int n_bindings;
	unsigned int *buf_size;
	void **cpu_buf;
	void **gpu_buf;
	bool *is_input;
};