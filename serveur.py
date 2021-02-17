# curl -X GET http://localhost:8888/Etudiant
# curl -X POST http://localhost:8888/Etudiant/\?Nom\=Cionaire\&Prenom\=Dick\&idAd\=2

# import http.server, urllib.parse, sqlite3
# import http.server, urllib.parse, sqlite3, threading, socketserver, requests, json
import http.server, urllib.parse, sqlite3, threading, socketserver, requests, json, random
import paho.mqtt.client as mqtt
def on_connect(client, userdata, flags, rc):
	print("Connected with result code: " + str(rc))

def on_message(client, userdata, msg):
	print(msg.topic + " " + str(msg.payload))


def hellopage():

	hellopage = """	<!DOCTYPE html>
					<html lang="en">
						<head>
							<title>IoT projet</title>
					  		<meta charset="utf-8" content="width=device-width, initial-scale=1">
					  		<link rel = "shortcut icon" href = "#"/>
					  		<style>
					  			h1 {
					  				text-align:center;
					  				color:green;
					  			}
					  			ul {
					    			list-style-type: none;
					    			margin: 0;
					    			padding: 0;
					    			overflow: hidden;
					    			background-color: #f1f1f1;
								}

								li {
					    			float: left;
								}

								li a {
					    			display: block;
					    			color: #000;
					    			text-align: center;
					    			padding: 14px 160px;
					    			text-decoration: none;
								}

								li a:hover {
					    			background-color: #555;
					    			color: white;
								}	

					  		</style>
						</head>
						<body>
					  		<h1>CrazyBaby</h1>
					  		<ul>
					  			<li><a href="http://172.20.10.8:8888/home">Home</a></li>
					  			<li><a href="http://172.20.10.8:8888/temprature">Temperature</a></li>
					  			<li><a href="http://172.20.10.8:8888/humidite">Humidite</a></li>
					  			<li><a href="http://192.168.43.94:8123">Video</a></li>
					  			<li><a href="http://172.20.10.8:8888/mouvement">Mouvement</a></li>
					  			<li><a href="http://172.20.10.8:8888/airPress">Air Pressure</a></li>
							</ul>
							<h2 style="text-align:center;">Led on/off</h2>
								<form action="index.html" method="post" style="text-align:center;">
								led(on/off)	: <input type="text" name="led">
								<input type="submit">
								</form>
						</body>
					</html>"""
	return hellopage


					  			# <li><a href="http://localhost:8888/configuration">Configuration</a></li>


def homePage(info):
	
	checketat = """<html>
  <head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['table']});
      google.charts.setOnLoadCallback(drawTable);
      function drawTable() {
        var data = new google.visualization.DataTable();
        data.addColumn('string', 'Name');
        data.addColumn('string', 'Tele');
        data.addColumn('string', 'Ip');
        data.addColumn('number', 'N');
        data.addColumn('string', 'Avenu');
        data.addColumn('string', 'Ville');
        data.addColumn('string', 'Post');
        data.addRows([%s]);
        var table = new google.visualization.Table(document.getElementById('table_div'));
        table.draw(data,{showRowNumber: true, width: '900px', height: '500px'});
      }
    </script>
  </head>
  <body>
  	<h1>User information</h1>
    <div id="table_div"></div>
  </body>
</html>""" % (info)
	return checketat
	

def temperaturePage(info):
	chart = """
	<html>
	  <head>
	    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	    <script type="text/javascript">
	      google.charts.load('current', {'packages':['corechart']});
	      google.charts.setOnLoadCallback(drawChart);

	      function drawChart() {
	        var data = google.visualization.arrayToDataTable([
	          ['Time', 'Temperature'],""" 
	chart += str(info)
	chart += """
	          ]);

	        var options = {
	          title: 'Temperatrure',
	          hAxis: {title: 'Time',  titleTextStyle: {color: '#333'}},
	          vAxis: {minValue: 0}
	        };

	        var chart = new google.visualization.AreaChart(document.getElementById('chart_div'));
	        chart.draw(data, options);
	      }
	    </script>
	  </head>
	  <body>
	    <div id="chart_div" style="width: 100%; height: 500px;"></div>
	  </body>
	</html>
	"""
	return chart


def humiditePage(info):
	chart = """
	<html>
	  <head>
	    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	    <script type="text/javascript">
	      google.charts.load('current', {'packages':['corechart']});
	      google.charts.setOnLoadCallback(drawChart);

	      function drawChart() {
	        var data = google.visualization.arrayToDataTable([
	          ['Time', 'Humidite'],""" 
	chart += str(info)
	chart += """
	          ]);

	        var options = {
	          title: 'Humidite',
	          hAxis: {title: 'Time',  titleTextStyle: {color: '#333'}},
	          vAxis: {minValue: 0}
	        };

	        var chart = new google.visualization.AreaChart(document.getElementById('chart_div'));
	        chart.draw(data, options);
	      }
	    </script>
	  </head>
	  <body>
	    <div id="chart_div" style="width: 100%; height: 500px;"></div>
	  </body>
	</html>
	"""
	return chart


	return chart

def mouvementPage(info):
	chart = """
	  <html>
	  <head>
	    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	    <script type="text/javascript">
	      google.charts.load('current', {'packages':['corechart']});
	      google.charts.setOnLoadCallback(drawChart);

	      function drawChart() {
	        var data = google.visualization.arrayToDataTable([
	          ['Time', 'AcX', 'AcY', 'AcZ'],""" 

	chart += str(info)        
	chart += """
			  ]);

	        var options = {
	          title: 'Mouvement',
	          curveType: 'function',
	          legend: { position: 'bottom' }
	        };

	        var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));

	        chart.draw(data, options);
	      }
	    </script>
	  </head>
	  <body>
	    <div id="curve_chart" style="width: 900px; height: 500px"></div>
	  </body>
	</html>
	"""
	return chart


def airPressPage(info):
	chart = """
	<html>
	  <head>
	    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
	    <script type="text/javascript">
	      google.charts.load('current', {'packages':['corechart']});
	      google.charts.setOnLoadCallback(drawChart);

	      function drawChart() {
	        var data = google.visualization.arrayToDataTable([
	          ['Time', 'Air pressure'],""" 
	chart += str(info)
	chart += """
	          ]);

	        var options = {
	          title: 'Air pressure',
	          hAxis: {title: 'Time',  titleTextStyle: {color: '#333'}},
	          vAxis: {minValue: 0}
	        };

	        var chart = new google.visualization.AreaChart(document.getElementById('chart_div'));
	        chart.draw(data, options);
	      }
	    </script>
	  </head>
	  <body>
	    <div id="chart_div" style="width: 100%; height: 500px;"></div>
	  </body>
	</html>
	"""
	return chart
def configurationPage():
	web_configuration = """
	<html>
		<body>
		<h2 style="text-align:center;">Ajouter le capteur</h2>
		<form action="index.html" method="post" style="text-align:center;">
		led(on/off)	: <input type="text" name="led">
		<input type="submit">
		</form>
		</body>
	</html>
	""";
	return web_configuration;

class MyHandler(http.server.BaseHTTPRequestHandler):
	def __init__(self, *args, **kwargs):
		self.mysql = MySQL('crazyBaby.db')
		super(MyHandler, self).__init__(*args, **kwargs)
		
	def do_GET(self):
		if self.path == '/favicon.ico':
			return 
		if self.path == '/':    #localhost:8888/
			self.send_response(200)
			self.send_header("Content-type", "text/html")
			self.end_headers()
			html  = hellopage()
			self.wfile.write(bytes(str(html)+'\n', 'UTF-8'))
		else :
			res = urllib.parse.urlparse(self.path)
			rep = self.mysql.select(res.path)
			if len(rep) > 0:
				self.send_response(200)
				self.send_header("Content-type", "text/html")
				self.end_headers()
				self.wfile.write(bytes(str(rep)+'\n', 'UTF-8'))
			else:
				self.send_response(404)
				self.send_header("Content-type", "text/html")
				self.end_headers()
		
	def do_POST(self):
		"""Respond to a POST request."""
		if self.path == "/index.html":
			q = self.rfile.read(int(self.headers['content-length'])).decode(encoding="utf-8")
			query = urllib.parse.parse_qs(q,keep_blank_values=1,encoding='utf-8')


			client = mqtt.Client()
			client.on_connect = on_connect
			client.on_message = on_message
			client.connect('172.20.10.8', 1883) 

			# print(query)
			# print(query['led'][0])
			if(query['led'][0] == "on"):
				print("led on")
				client.publish('ledOnOff', payload='4')
			elif(query['led'][0] == "off"):
				print("led off")
				client.publish('ledOnOff', payload='5')


			

			# 
			



		else :
			res = urllib.parse.urlparse(self.path)
			query = urllib.parse.parse_qs(res.query)
			rep = self.mysql.insert(res.path,query)
		self.send_response(200)
		self.send_header("Content-type", "text/html")
		self.end_headers()

class MySQL():
	def __init__(self, name):
		self.c = None
		self.req = None
		self.conn = sqlite3.connect(name)
		self.c = self.conn.cursor()

	def __exit__(self, exc_type, exc_value, traceback):
		self.conn.close()

	def select(self,path):


		elem = path.split('/')
		if elem[1] == "home" :

			req = "select * from Logement"

			logeInfo = self.c.execute(req).fetchall()
		
			req = "select * from Address where id = %d" % (logeInfo[0][4])

			addInfo = self.c.execute(req).fetchall()
		
			req = "select * from Poste where code = %s" % (addInfo[0][3])

			postInfo = self.c.execute(req).fetchall()
		
			info = "[\'%s\',\'%s\',\'%s\',%d,\'%s\',\'%s\',\'%s\']" % \
				(logeInfo[0][1],\
				logeInfo[0][2],\
				logeInfo[0][3],\
				addInfo[0][1],\
				addInfo[0][2],\
				postInfo[0][0],\
				postInfo[0][1])
			print(info)


			return homePage(info)
		elif elem[1] == "temprature" :

			req = "select * from TempHumi"

			tempInfo = self.c.execute(req).fetchall()

			info = ""

			infoSize = 0

			for i in tempInfo :
				infoSize += 1

			for i in range(infoSize) :
				info += "[\'%s\',%d]" % (tempInfo[i][4], tempInfo[i][1])
				if(i < infoSize - 1) :
					info += ','
			
			return temperaturePage(info)
		elif elem[1] == "humidite" :

			req = "select * from TempHumi"

			tempInfo = self.c.execute(req).fetchall()

			info = ""

			infoSize = 0

			for i in tempInfo :
				infoSize += 1

			for i in range(infoSize) :
				info += "[\'%s\',%d]" % (tempInfo[i][4], tempInfo[i][2])
				if(i < infoSize - 1) :
					info += ','
			
			return humiditePage(info)
		elif elem[1] == "mouvement" :
			req = "select * from Mouvement"
			mouveInfo = self.c.execute(req).fetchall()

			info = ""

			infoSize = 0

			for i in mouveInfo :
				infoSize += 1

			for i in range(infoSize) :
				info += "[\'%s\',%d, %d, %d]" % (mouveInfo[i][5], mouveInfo[i][1], mouveInfo[i][2],mouveInfo[i][3])
				if(i < infoSize - 1) :
					info += ','

			return mouvementPage(info)

		elif elem[1] == "airPress" :
			req = "select * from AirPressure"
			airPreseInfo = self.c.execute(req).fetchall()

			info = ""

			infoSize = 0

			for i in airPreseInfo :
				infoSize += 1

			for i in range(infoSize) :
				info += "[\'%s\',%d]" % (airPreseInfo[i][3], airPreseInfo[i][1])
				if(i < infoSize - 1) :
					info += ','

			return airPressPage(info)


		elif elem[1] == "configuration":

			return configurationPage();
		


		else : 
			if len(elem) == 2:
				req = "select * from %s" %(elem[1])
			else:
				req = "select %s from %s where id=%s" %(elem[3],elem[1],elem[2])
			return self.c.execute(req).fetchall()
	
	def insert(self,path,query):
		print(query)
		attr = ', '.join(query.keys())
		val = ', '.join('"%s"' %v[0] for v in query.values())
		print(attr,val)
		req = "insert into %s (%s) values (%s)" %(path.split('/')[1], attr, val)
		print(req)
		self.c.execute(req)
		self.conn.commit()

if __name__ == '__main__':
	server_class = http.server.HTTPServer
	httpd = server_class(("172.20.10.8", 8888), MyHandler)
	try:
		httpd.serve_forever()
	except KeyboardInterrupt:
		pass
	httpd.server_close()