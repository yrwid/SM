<?php
class bmp180{
 public $link='';
 function __construct($temperature_bmp, $humidity_bmp){
  $this->connect();
  $this->storeInDB($temperature_bmp, $humidity_bmp);
 }
 
 function connect(){
  $this->link = mysqli_connect('localhost','root','') or die('Cannot connect to the DB');
  mysqli_select_db($this->link,'projekt_it') or die('Cannot select the DB');
 }
 
 function storeInDB($temperature_bmp, $humidity_bmp){
  $query = "insert into bmp180 set humidity_bmp='".$humidity_bmp."', temperature_bmp='".$temperature_bmp."'";
  $result = mysqli_query($this->link,$query) or die('Errant query:  '.$query);
 }
 
}
if($_GET['temperature_bmp'] != '' and  $_GET['humidity_bmp'] != ''){
 $bmp180=new bmp180($_GET['temperature_bmp'],$_GET['humidity_bmp']);
}


?>
