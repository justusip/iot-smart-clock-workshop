import datetime
import flask
import pytz

app = flask.Flask(__name__)
tz = pytz.timezone("Asia/Hong_Kong")


@app.route("/")
def status():
    return {
        "status": "ok"
    }


@app.route("/raw/time")
def time():
    return datetime.datetime.now(tz).strftime("%H:%M %p")


@app.route("/raw/date")
def date():
    return datetime.datetime.now(tz).strftime("%d/%m/%Y")


@app.route("/raw/temperature")
def temperature():
    # req = requests.get(
    #     "https://api.open-meteo.com/v1/forecast?latitude=22.2783&longitude=114.1747&current=temperature_2m&timezone=auto")
    # return f"{req.json()['current']['temperature_2m']} 'C"
    return "26.1 'C"


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)
