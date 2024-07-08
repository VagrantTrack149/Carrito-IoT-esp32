#include <WiFi.h>
#include <WebServer.h>
#include <HCSR04.h>
WebServer server(80);
byte triggerPin = 13;
byte echoPin = 12;

void onRoot(){
  String pagina=R"(<html lang='en'>
<head>
    <style>
        button {
            background-color: blue; 

            color: white; 
            padding: 20px 30px;
            border: none; 
            cursor: pointer; 
            text-align: center; 
            display: inline-block; 
            margin: 5px; 
            font-size: 20px; 
            
            }

        button:active {
            background-color: red; 
            }
        input[type='range'] {
            appearance: slider-vertical;
        }
        #slidder{
            position: absolute;
            left: 0%;
            height: 80%;
            top: 0%;   
        }
        #slidder1{
            position: absolute;
            left: 90%;
            height: 80%; 
            top: 0%;
        }
        #izq{
            position: absolute;
            left: 5%;
            font-size: 20px;
            top: 0%;
        }
        #der{
            position: absolute;
            left: 89%;
            font-size: 20px;
            top: 0%;
        }
        #status{
            position: absolute;
            left: 46%;
            top: 80%;
        }
        #camara{
            border: 1px solid black;
            width: 60%;
            height: 70%;
            position: absolute;
            left: 20%;
        }
        #distancia{
            left: 46%;
            font-size: 20px;
            position: absolute;
            top: 90%;
        }

    </style>
    <script>
        function izquierda_rango(){
            document.getElementById('status').innerHTML='moviendose';
            var req = new XMLHttpRequest();
            req.open('get','/izquierda_rango?vel='+document.getElementById('slidder').value);
            req.send();

        }
        function derecha_rango(){
            document.getElementById('status').innerHTML='moviendose';
            var req = new XMLHttpRequest();
            req.open('get','/derecha_rango?vel='+document.getElementById('slidder1').value);
            req.send();

        }
        function dejar_up(){
        document.getElementById('slidder').value=0;
        document.getElementById('status').innerHTML='detenido';
        var req = new XMLHttpRequest();
        req.open('get','/izquierda_rango?vel='+document.getElementById('slidder').value);
        req.send();
        }
        function dejar_up1(){
        document.getElementById('slidder1').value=0;
        document.getElementById('status').innerHTML='detenido';
        var req = new XMLHttpRequest();
        req.open('get','/derecha_rango?vel='+document.getElementById('slidder1').value);
        req.send();
        }
        function datos(){
            setInterval(getSensor,200)
        }
        function getSensor(){
            var req= new XMLHttpRequest();
            req.open('get','/getSensor');
            req.onreadystatechange=function(){
                if (req.status==200 && req.readyState==4) {
                    document.getElementById('distancia').innerHTML= req.responseText;
                }
            }
            req.send();
        }
        function delante(){
            if (event.key=='w') {
                document.getElementById('slidder1').value=255;
                document.getElementById('slidder').value=255;
                document.getElementById('status').innerHTML='delante';
                var req = new XMLHttpRequest();
                req.open('get','/delante');
                req.send();   
            }
        }
        function dejar(){
            document.getElementById('slidder1').value=0;
            document.getElementById('slidder').value=0;
            document.getElementById('status').innerHTML='stop';
            var req = new XMLHttpRequest();
            req.open('get','/dejar');
            req.send(); 
        }
    </script>
</head>
<body onload='datos();' onkeydown='delante()' onkeyup='dejar()'>
    <input id='slidder' type='range' min='-255' max='255' oninput='izquierda_rango()' onmouseup='dejar_up()'>
    <br>
    <h4 id='izq'>Izquierda</h4>
    <h4 id='der'>Derecha</h4>
    <input id='slidder1' type='range' min='-255' max='255' oninput='derecha_rango()' onmouseup='dejar_up1()'> 
    <div id='status'></div>
    <canvas id='camara'></canvas>
    
    <div id='distancia' >0.0cm</div>
</body>
</html>)";
  server.send(200,"text/html",pagina);
}
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  /*  pinMode(12,OUTPUT); //Positivo izq
    pinMode(13,OUTPUT); //Negativo izq
    pinMode(14,OUTPUT); //Positivo Der
    pinMode(27,OUTPUT); //Negativo Der
    digitalWrite(12,LOW);
    digitalWrite(13,LOW);
    digitalWrite(14,LOW);
    digitalWrite(27,LOW);*/

  ledcSetup(0,5000,8);
  ledcAttachPin(14,0);
  ledcSetup(1,5000,8);
  ledcAttachPin(15,1);
  ledcSetup (2,5000,8);
  ledcAttachPin(4,2);
  ledcSetup(3,5000,8);
  ledcAttachPin(2,3);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Mi_red","");
  /*while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }*/
  Serial.println(".");
  Serial.println(WiFi.softAPIP());
  server.on("/",onRoot);
  server.on("/izquierda_rango",izquierda_rango);
  server.on("/derecha_rango",derecha_rango);
  server.on("/getSensor",getSensor);
  server.on("/delante",delante);
  server.on("/dejar",dejar);
  server.begin();
  HCSR04.begin(triggerPin, echoPin);
  }
void delante(){
  ledcWrite(2,255);
  ledcWrite(3,0);
  ledcWrite(0,255);
  ledcWrite(1,0);
  server.send(200,"text/html","");
}
void dejar(){
  ledcWrite(2,0);
  ledcWrite(3,0);
  ledcWrite(0,0);
  ledcWrite(1,0);
  server.send(200,"text/html","");
}
//derecha slidder
void derecha_rango(){
  int vel=server.arg("vel").toInt();
  Serial.println(vel);
  if(vel>=0){
    ledcWrite(2,vel);
    ledcWrite(3,0);
  }else{
    vel=vel*-1;
    ledcWrite(2,0);
    ledcWrite(3,vel);
  }
  server.send(200,"text/html","");
}
//izquierda slidder
void izquierda_rango(){
  int vel=server.arg("vel").toInt();
  Serial.println(vel);
  if(vel>=0){
    ledcWrite(0,vel);
    ledcWrite(1,0);
  }else{
    vel=vel*-1;
    ledcWrite(1,0);
    ledcWrite(1,vel);
  } 
  server.send(200,"text/html","");
}
void getSensor(){
  double* distances = HCSR04.measureDistanceCm();
  Serial.print(distances[0]);
  Serial.println(" cm");
  server.send(200,"text/html",String(distances[0]));
}

void loop() {
  server.handleClient();
  // put your main code here, to run repeatedly:
  /*digitalWrite(12,HIGH);
  digitalWrite(13,LOW);
  digitalWrite(14,HIGH);
  digitalWrite(27,LOW);*/
  }
