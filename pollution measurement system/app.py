from flask import Flask, request, jsonify, render_template, send_file

from flask_cors import CORS

import mysql.connector

import pandas as pd



app = Flask(__name__)

CORS(app)



db = mysql.connector.connect(

    host="localhost",

    user="root",

    password="aditri_1428",

    database="pollution_db"

)



@app.route("/")

def home():

    return render_template("index.html")





@app.route("/dashboard")

def dashboard():

    return render_template("index.html")





@app.route("/data", methods=["POST"])

def receive_data():



    try:



        mq2 = int(request.form.get("mq2", 0))

        mq3 = int(request.form.get("mq3", 0))

        mq135 = int(request.form.get("mq135", 0))



        avg_sensor = (mq2 + mq3 + mq135) / 3

        aqi = int((avg_sensor / 4095.0) * 500)



        if aqi <= 50:

            status = "Good"

        elif aqi <= 100:

            status = "Satisfactory"

        elif aqi <= 200:

            status = "Moderate"

        elif aqi <= 300:

            status = "Poor"

        elif aqi <= 400:

            status = "Very Poor"

        else:

            status = "Severe"



        print("===================================")

        print("MQ2 :", mq2)

        print("MQ3 :", mq3)

        print("MQ135 :", mq135)

        print("AQI :", aqi)

        print("STATUS :", status)

        print("===================================")



        cursor = db.cursor()



        sql = """

        INSERT INTO sensor_data

        (mq2, mq3, mq135, aqi_status)

        VALUES (%s, %s, %s, %s)

        """



        cursor.execute(

            sql,

            (mq2, mq3, mq135, status)

        )



        db.commit()



        return jsonify({

            "message": "Stored",

            "aqi": aqi,

            "status": status

        })



    except Exception as e:



        print("ERROR:", e)



        return jsonify({

            "error": str(e)

        }), 500





@app.route("/latest")

def latest():



    try:



        cursor = db.cursor()



        cursor.execute("""

        SELECT mq2,mq3,mq135,aqi_status,timestamp

        FROM sensor_data

        ORDER BY id DESC

        LIMIT 1

        """)



        row = cursor.fetchone()



        if row:



            avg_sensor = (

                row[0] +

                row[1] +

                row[2]

            ) / 3



            aqi = int(

                (avg_sensor / 4095.0) * 500

            )



            return jsonify({



                "mq2": row[0],

                "mq3": row[1],

                "mq135": row[2],

                "aqi": aqi,

                "status": row[3],

                "time": str(row[4])



            })



        return jsonify({



            "mq2": 0,

            "mq3": 0,

            "mq135": 0,

            "aqi": 0,

            "status": "Waiting",

            "time": ""



        })



    except Exception as e:



        return jsonify({

            "error": str(e)

        })





@app.route("/history")

def history():



    try:



        cursor = db.cursor()



        cursor.execute("""

        SELECT *

        FROM sensor_data

        ORDER BY id DESC

        LIMIT 100

        """)



        rows = cursor.fetchall()



        result = []



        for row in rows:



            avg_sensor = (

                row[2] +

                row[3] +

                row[4]

            ) / 3



            aqi = int(

                (avg_sensor / 4095.0) * 500

            )



            result.append({



                "id": row[0],

                "time": str(row[1]),

                "mq2": row[2],

                "mq3": row[3],

                "mq135": row[4],

                "aqi": aqi,

                "status": row[5]



            })



        return jsonify(result)



    except Exception as e:



        return jsonify({

            "error": str(e)

        })





@app.route("/download_csv")

def download_csv():



    query = """

    SELECT *

    FROM sensor_data

    """



    df = pd.read_sql(

        query,

        db

    )



    csv_file = "sensor_data.csv"



    df.to_csv(

        csv_file,

        index=False

    )



    return send_file(

        csv_file,

        as_attachment=True

    )





@app.route("/login")

def login():

    return render_template("login.html")





@app.route("/signup")

def signup():

    return render_template("signup.html")





if __name__ == "__main__":



    app.run(

        host="0.0.0.0",

        port=5000,

        debug=True

    )