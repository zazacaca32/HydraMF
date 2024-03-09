#define Akey "hydra"
#define whoami "hydra"

void sendJsonToDB()
{
              
  String json;
  json  =  "{\"system\":{ ";
  json += "\"Akey\":\"" + String(Akey) + "\",";
  json += "\"Serial\":\"" + String(whoami) + "\", ";
  json += "\"Version\":\"2024-03-09\", ";
  json += "\"RSSI\":\"" + String(WiFi.RSSI()) + "\",";
  json += "\"MAC\":\"" + String(WiFi.macAddress()) + "\",";
  json += "\"IP\":\""; json+= getDeviceIp().toString(); json += "\"},";

  json += "\"weather\":{ ";
  json += "\"temp\": \""+String(temp)+"\",";
  json += "\"pressure\": \""+String(pressure)+"\",";
  json += "\"humidity\": \""+String(humidity)+"\" }}"; 

  println(json);

  http.begin(client, "http://dbrobo.mf.bmstu.ru/core/jsonadd.php");
  http.addHeader("Content-Type", "application/json");
  int resulthttp = http.POST(json);
  println("Done: ");
  println(String(resulthttp));
  http.end();
}