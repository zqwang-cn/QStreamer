import subprocess
from flask import Flask, request, redirect, render_template

app = Flask(import_name=__name__,
            static_url_path='/',
            static_folder='config_creator_js/public',
            template_folder='config_creator_js/public')


@app.route("/")
def index():
    return redirect('/index.html')


@app.route("/save", methods=['POST'])
def save():
    config = request.get_data()
    with open('config.json', 'w') as f:
        f.write(config.decode('utf8'))
    subprocess.Popen(['build/main', 'config.json'])
    return 'success'


@app.route("/play")
def play():
    return render_template('player.html', uri=request.args.get('uri'))


if __name__ == '__main__':
    app.run(debug=True)
