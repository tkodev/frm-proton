/*
Misc HTKO functions
*/

#include "application.h"
#include "lib_htko.h"

htko::htko(int woo){
  // This is where the constructor would be...right now we are too stupid to have one
}

String htko::strDigit(int inputInt){
  return ( inputInt ? "on" : "off" );
}

String htko::strBool(boolean inputBool){
  String result = "";
  if( inputBool<=1 ){
    result = ( inputBool ? "On" : "Off" );
  }else if( inputBool==2 ){
    result = "Toggled";
  }
  return result;
}
String htko::strBool2(boolean inputBool){
  return ( inputBool ? "Success" : "Failed" );
}
int htko::digitStr(String inputStr){
  return ( inputStr=="on" ? 1 : 0 );
}
int htko::cmdStr(String inputStr){
    if( inputStr=="light" ){ // cmd1
      return 1;
    }else if( inputStr=="temp" ){
      return 2;
    }else if( inputStr=="reset" ){
      return 3;
    }else if( inputStr=="ir" ){
      return 4;
    }else if( inputStr=="garage" ){
      return 5;
    }else if( inputStr=="off" ){ // light.cmd2
      return 0;
    }else if( inputStr=="on" ){
      return 1;
    }else if( inputStr=="toggle" ){
      return 2;
    }else if( inputStr=="report" ){ // temp.cmd2
      return 1;
    }else if( inputStr=="device" ){ // reset.cmd2
      return 1;
    }else if( inputStr=="flash" ){ // reset.cmd2
      return 1;
    }else if( inputStr=="door" ){
      return 1;
    }else if( inputStr=="all" ){ // light.cmd3
      return 3;
    }else{
      return atoi(inputStr);
    }
}
String htko::cap(String inputStr){
  if( inputStr!="" ){
    inputStr.setCharAt(0, toupper(inputStr.charAt(0)) );
  }
  return inputStr;
}
String htko::lowcap(String inputStr){
  if( inputStr!="" ){
    inputStr.setCharAt(0, tolower(inputStr.charAt(0)) );
  }
  return inputStr;
}
int htko::countIntArray (int data[])
{
    int count = sizeof(data)/sizeof(data[0]);
    return count;
}
