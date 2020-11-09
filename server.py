import os
from flask import Flask, request, redirect

app = Flask(import_name=__name__,
            static_url_path='/',
            static_folder='config_creator_js/public')


@app.route("/")
def index():
    return redirect('/index.html')


@app.route("/save", methods=['POST'])
def save():
    config = request.get_data()
    with open('config.json', 'w') as f:
        f.write(config.decode('utf8'))
    os.system('build/main config.json')
    return 'success'


if __name__ == '__main__':
    app.run(debug=True)
