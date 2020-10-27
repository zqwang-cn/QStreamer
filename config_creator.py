import sys
import json
import os
import copy
import graphviz
from PyQt5.QtWidgets import QApplication, QWidget, QComboBox, QGridLayout, QLabel, QPushButton, QLineEdit
from PyQt5.QtGui import QPixmap


def render(config):
    g = graphviz.Digraph('config', format='png', graph_attr={'rankdir': "LR"}, node_attr={'shape': 'record'})

    for ele_name, element in config['elements'].items():
        in_pads = element['in_pads']
        out_pads = element['out_pads']

        in_txts = ['<' + name + '>' + name for name in in_pads]
        in_txt = '{' + '|'.join(in_txts) + '}'
        out_txts = ['<' + name + '>' + name for name in out_pads]
        out_txt = '{' + '|'.join(out_txts) + '}'
        txt = '{' + in_txt + '|' + ele_name + '|' + out_txt + '}'
        g.node(ele_name, txt)

    for link in config['links']:
        fr_ele_name = link['from']['element_name']
        fr_pad_name = link['from']['pad_name']
        to_ele_name = link['to']['element_name']
        to_pad_name = link['to']['pad_name']
        fr = fr_ele_name+':'+fr_pad_name
        to = to_ele_name+':'+to_pad_name
        g.edge(fr, to)

    g.render(cleanup=True)


def render_element(ele_name, element):
    g = graphviz.Digraph('element', format='png', graph_attr={'rankdir': "LR"}, node_attr={'shape': 'record'})
    in_pads = element['in_pads']
    out_pads = element['out_pads']

    in_txts = ['<' + name + '>' + name for name in in_pads]
    in_txt = '{' + '|'.join(in_txts) + '}'
    out_txts = ['<' + name + '>' + name for name in out_pads]
    out_txt = '{' + '|'.join(out_txts) + '}'
    txt = '{' + in_txt + '|' + ele_name + '|' + out_txt + '}'
    g.node(ele_name, txt)
    g.render(cleanup=True)


config = {
    'elements': {},
    'links': []
}
with open('elements.json') as f:
    defs = json.load(f)


class ConfigWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.def_list = QComboBox(self)
        self.def_list.currentIndexChanged.connect(self.def_list_text_changed)
        self.ele_name_edit = QLineEdit(self)
        self.ele_preview_label = QLabel('ele_preview', self)
        self.add_button = QPushButton('add', self)
        self.add_button.clicked.connect(self.add_button_clicked)

        self.fr_ele_list = QComboBox(self)
        self.fr_pad_list = QComboBox(self)
        self.to_ele_list = QComboBox(self)
        self.to_pad_list = QComboBox(self)
        self.fr_ele_list.currentTextChanged.connect(self.fr_ele_list_text_changed)
        self.to_ele_list.currentTextChanged.connect(self.to_ele_list_text_changed)
        self.link_button = QPushButton('link', self)
        self.link_button.clicked.connect(self.link_button_clicked)

        self.ele_list = QComboBox(self)
        self.ele_list.currentTextChanged.connect(self.ele_list_text_changed)
        self.property_list = QComboBox(self)
        self.property_list.currentTextChanged.connect(self.property_list_text_changed)
        self.property_edit = QLineEdit(self)
        self.set_property_button = QPushButton('set', self)
        self.set_property_button.clicked.connect(self.set_property_button_clicked)

        self.export_button = QPushButton('export', self)
        self.export_button.clicked.connect(self.export_button_clicked)

        self.preview_label = QLabel('preview', self)

        layout = QGridLayout()
        layout.addWidget(self.def_list, 0, 0)
        layout.addWidget(self.ele_name_edit, 0, 1)
        layout.addWidget(self.ele_preview_label, 0, 2)
        layout.addWidget(self.add_button, 0, 3)
        layout.addWidget(self.fr_ele_list, 1, 0)
        layout.addWidget(self.fr_pad_list, 1, 1)
        layout.addWidget(self.to_ele_list, 1, 2)
        layout.addWidget(self.to_pad_list, 1, 3)
        layout.addWidget(self.link_button, 1, 4)
        layout.addWidget(self.ele_list, 2, 0)
        layout.addWidget(self.property_list, 2, 1)
        layout.addWidget(self.property_edit, 2, 2)
        layout.addWidget(self.set_property_button, 2, 3)
        layout.addWidget(self.export_button, 2, 4)
        layout.addWidget(self.preview_label, 3, 0, 1, 5)
        self.setLayout(layout)

        for name, _ in defs.items():
            self.def_list.addItem(name)
        self.refresh()

    def def_list_text_changed(self):
        name = self.def_list.currentText()
        render_element(name, defs[name])
        self.ele_preview_label.setPixmap(QPixmap('element.gv.png'))

    def fr_ele_list_text_changed(self):
        if self.fr_ele_list.currentIndex() == -1:
            return
        self.fr_pad_list.clear()
        for pad_name, _ in config['elements'][self.fr_ele_list.currentText()]['out_pads'].items():
            self.fr_pad_list.addItem(pad_name)

    def to_ele_list_text_changed(self):
        if self.to_ele_list.currentIndex() == -1:
            return
        self.to_pad_list.clear()
        for pad_name, _ in config['elements'][self.to_ele_list.currentText()]['in_pads'].items():
            self.to_pad_list.addItem(pad_name)

    def ele_list_text_changed(self):
        if self.ele_list.currentIndex() == -1:
            return
        self.property_list.clear()
        for property in config['elements'][self.ele_list.currentText()]['properties']:
            self.property_list.addItem(property['name'])

    def property_list_text_changed(self):
        for property in config['elements'][self.ele_list.currentText()]['properties']:
            if property['name'] == self.property_list.currentText():
                self.property_edit.setText(property['value'])
                return
        self.property_edit.setText('')

    def add_button_clicked(self):
        name = self.ele_name_edit.text()
        type = self.def_list.currentText()
        config['elements'][name] = copy.deepcopy(defs[type])
        self.refresh()

    def link_button_clicked(self):
        fr_ele_name = self.fr_ele_list.currentText()
        fr_pad_name = self.fr_pad_list.currentText()
        to_ele_name = self.to_ele_list.currentText()
        to_pad_name = self.to_pad_list.currentText()
        config['links'].append({
            "from": {
                "element_name": fr_ele_name,
                "pad_name": fr_pad_name
            },
            "to": {
                "element_name": to_ele_name,
                "pad_name": to_pad_name
            }
        })
        self.preview()

    def set_property_button_clicked(self):
        name = self.ele_list.currentText()
        for property in config['elements'][name]['properties']:
            if property['name'] == self.property_list.currentText():
                property['value'] = self.property_edit.text()
                return

    def export_button_clicked(self):
        with open('config.json', 'w') as f:
            json.dump(config, f, indent=4)

    def preview(self):
        render(config)
        self.preview_label.setPixmap(QPixmap('config.gv.png'))

    def refresh(self):
        self.fr_ele_list.clear()
        self.to_ele_list.clear()
        self.ele_list.clear()
        for name, _ in config['elements'].items():
            self.fr_ele_list.addItem(name)
            self.to_ele_list.addItem(name)
            self.ele_list.addItem(name)
        self.preview()

    def closeEvent(self, event):
        for filename in ['config.gv.png','element.gv.png']:
            if os.path.exists(filename):
                os.remove(filename)
        event.accept()


app = QApplication(sys.argv)
win = ConfigWidget()
win.show()
app.exit(app.exec_())