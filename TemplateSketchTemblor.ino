
/* Lee datos del IMU04 y los reporta
 *  como si fueran generados por un Field Sensor en 
 *  un Sistema de Alerta Sismica
 */

#include <Bridge.h>
    #include <Process.h>  
    #include <Wire.h>
    #include <LSM303.h>
    
    // apcicipn
    
    LSM303 compass;
    int ejex, ejey, ejez;
    int mascara = 65520; 
    String var, add, method, header,json;
    bool bandera;
    bool temblor = false;

    
    //Cambiar estos valores
    String zona  = "";
    String id = "";
    String dirIP = ":1026";
    
    
    void setup()
    {
      // Las funciones de configuración van aquí 

      
       delay(1000);
    }
    
    void loop()
    {
      
      // Llama aquí la función para encender el LED
      
      //Las funciones de acción van aquí 


      delay(2000);
      
      if(temblor){
         // ¿Qué debes hacer si se detecta un temblor?
      }     
    }





    //########################################################################################################
    // Funciones a utilizar

    //Fucnión que enciende LED
    void encenderLED(){
      digitalWrite(LED_BUILTIN, HIGH);   
      delay(1000);                       
      digitalWrite(LED_BUILTIN, LOW);    
    }
    
    // Función que enciende el sensor para utilizarlo
    void encenderSensor(){
      Serial.begin(9600);
      Bridge.begin();
      Wire.begin();
      compass.init();
      compass.enableDefault();
      pinMode(LED_BUILTIN, OUTPUT);
    }

    //Función que lee los datos del sensor y los imprime
    void leerDatos(){
      compass.read();
      
      ejex = (compass.a.x & mascara)>>4;
      ejey = (compass.a.y & mascara)>>4;
      ejez = (compass.a.z & mascara)>>4;
      
      if(ejex<0)
        ejex = -ejex;
      if(ejey<0)
        ejey = -ejey;
      if(ejez<0)
        ejez = -ejez;
      Serial.println(ejex);
      Serial.println(ejey);
    }

    
   // Función que comprueba si hay un temblor
    void comprobarTemblor(){
       if(ejex >= 500){
        Serial.println("temblor trepidatorio sobre x");
        enviarDatosTemblor(ejex);
        temblor = true;
        bandera = false;
       }
          
      if(ejey>= 500&& bandera){
        Serial.println("temblor trepidatorio sobre y");
        enviarDatosTemblor(ejey);
        temblor = true;
        
      }
      bandera = true;
    }


    //Función que envía la magnitud de un temblor
    void enviarDatosTemblor(int magnitud){
        Process p;
        p.begin("curl");
        p.addParameter("-H");
        p.addParameter("Content-Type: application/json");
        p.addParameter("-X");
        p.addParameter("PUT");
        p.addParameter("-d");
        json = "{\"value\":"+(String)magnitud+"}";
        Serial.println(json);
        p.addParameter(json);
        p.addParameter("-k");
        p.addParameter("http://"+dirIP+"/v2/entities/"+id+"/attrs/temblor");
        p.run();
        while (p.available() > 0) {
          char c = p.read();
          Serial.print(c);
        }
        Serial.println("Terminé enviar datos");
        Serial.flush();
        p.close();
    }


        //Función que regresa los valores del temblor a cero para poder recibir nuevos datos. 
        void regresarValoresIniciales(){
        Process p;
        temblor = false;
        p.begin("curl");
        p.addParameter("-H");
        p.addParameter("Content-Type: application/json");
        p.addParameter("-X");
        p.addParameter("PUT");
        p.addParameter("-d");
        json = "{\"value\":0}";
        Serial.println(json);
        p.addParameter(json);
        p.addParameter("-k");
        p.addParameter("http://"+dirIP+"/v2/entities/"+id+"/attrs/temblor");        p.run();
        while (p.available() > 0) {
          char c = p.read();
          Serial.print(c);
        }
        Serial.flush();
        p.close();
    }


    //Función que registra al sensor en el servidor
    void registrarSensor(){
        Process p;
        Serial.println("Registro de sensor");
        p.begin("curl");
        p.addParameter("-H");
        p.addParameter("Content-Type: application/json");
        p.addParameter("-X");
        p.addParameter("POST");
        p.addParameter("-d");
        p.addParameter("{\"id\": "+id+",\"type\":\"SensorSismico\",\"temblor\":{\"value\":0},\"zona\":{\"value\":"+zona+"}}");
        p.addParameter("-k");
        p.addParameter("http://"+dirIP+"/v2/entities");
        p.run();
        while (p.available() > 0) {
          char c = p.read();
          Serial.print(c);
        }
        Serial.println("Terminé regsitro");
        Serial.flush();
        p.close();
        return;
    }
    