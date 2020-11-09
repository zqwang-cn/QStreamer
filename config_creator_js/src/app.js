import elements from "./elements.json"
import $ from "jquery"
import jsPlumb from "jsplumb"

var $jsPlumb_ele = jsPlumb.jsPlumb.getInstance({
    Container: "container-ele"
});
var $jsPlumb_con = jsPlumb.jsPlumb.getInstance({
    Container: "container-con"
});

var in_endpoint_commons = {
    connector: ['Straight'],
    paintStyle: {
        fill: 'green',
    },
    connectorOverlays: [
        ["Arrow", { location: 1 }]
    ]
};
var out_endpoint_commons = {
    connector: ['Straight'],
    paintStyle: {
        fill: 'blue',
    },
    connectorOverlays: [
        ["Arrow", { location: 1 }]
    ]
};

var node_count = {};
var nodes = {};
var endpoints = {};
var connections = {};
var current_name;

function add_node(jsPlumb, type, node_name, draggable) {
    var node_id = "n-" + node_name;
    var node_html = `<div>
                       <div class="node-label">${node_name}</div>
                       <div class="node-inner" id="${node_id}">
                       </div>
                     </div>`
    var node = $(node_html);
    if (draggable) {
        node.addClass("node-con");
        $("#container-con").append(node);
    }
    else {
        node.addClass("node-ele");
        $("#container-ele").append(node);
    }

    var element = elements[type];
    var in_pads = element["in_pads"];
    var in_pad_names = Object.keys(in_pads);
    var n_in_pads = in_pad_names.length;
    for (var i = 0; i < n_in_pads; i++) {
        var ep = jsPlumb.addEndpoint(node_id, {
            anchor: [0, (1 / (n_in_pads + 1)) * (i + 1), -1, 0],
            isSource: false,
            isTarget: draggable,
            overlays: [
                ["Label", { label: in_pad_names[i], location: [-1, 0.5] }]
            ],
        }, in_endpoint_commons);
        if (draggable) {
            endpoints[ep.id] = {
                element_name: node_name,
                pad_name: in_pad_names[i]
            }
        }
    }

    var out_pads = element["out_pads"];
    var out_pad_names = Object.keys(out_pads);
    var n_out_pads = out_pad_names.length;
    for (var i = 0; i < n_out_pads; i++) {
        var ep = jsPlumb.addEndpoint(node_id, {
            anchor: [1, (1 / (n_out_pads + 1)) * (i + 1), 1, 0],
            isSource: draggable,
            isTarget: false,
            overlays: [
                ["Label", { label: out_pad_names[i], location: [2, 0.5] }]
            ],
        }, out_endpoint_commons);
        if (draggable) {
            endpoints[ep.id] = {
                element_name: node_name,
                pad_name: out_pad_names[i]
            }
        }
    }

    if (draggable) {
        jsPlumb.draggable(node, { containment: 'parent' });

        nodes[node_name] = JSON.parse(JSON.stringify(element));;

        node.on('click', function () {
            current_name = node_name;
            var properties = nodes[node_name].properties;
            $("#properties").empty();
            for (var name in properties) {
                var value = properties[name].value;
                var id = "p-" + name;
                var item = `\
                      <div class="form-group">
                        <label>
                          ${name}
                        </label>
                        <input type="text" class="form-control" id="${id}" value=${value}>
                      </div>`
                $("#properties").append(item);
            }
        });
    }
    else {
        node.on('dblclick', function () {
            if (node_count[type] === undefined) {
                node_count[type] = 0;
            }
            node_count[type]++;
            var node_name = type + '-' + node_count[type];
            add_node($jsPlumb_con, type, node_name, true);
        });
    }
}

function add_connection(info) {
    connections[info.connection.id] = info.connection;
}

function del_connection(info) {
    delete connections[info.connection.id];
}

function save() {
    if (current_name === undefined)
        return;

    var properties = nodes[current_name].properties;
    for (name in properties) {
        properties[name].value = $("#p-" + name).val();
    }
}

function export_json() {
    var config = {
        "elements": nodes,
        "links": []
    };

    for (var id in connections) {
        var connection = connections[id];
        var from = endpoints[connection.endpoints[0].id];
        var to = endpoints[connection.endpoints[1].id];
        config.links.push({
            "from": {
                "element_name": from.element_name,
                "pad_name": from.pad_name
            },
            "to": {
                "element_name": to.element_name,
                "pad_name": to.pad_name
            }
        })
    }

    var data = JSON.stringify(config, "", "\t");
    $.post("http://127.0.0.1:5000/save", data, function (data, status) {
        console.log(data);
    });
}

$jsPlumb_ele.ready(function () {
    for (var type in elements) {
        add_node($jsPlumb_ele, type, type, false);
    }
});

$jsPlumb_con.ready(function () {
    $jsPlumb_con.bind('connection', add_connection);
    $jsPlumb_con.bind('connectionDetached', del_connection);
    $("#save").on("click", save);
    $("#export").on("click", export_json);
});