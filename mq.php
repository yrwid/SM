<?php
class mq7{
 public $link='';
 function __construct($ppm){
  $this->connect();
  $this->storeInDB($ppm);
 }
 
 function connect(){
  $this->link = mysqli_connect('localhost','root','') or die('Cannot connect to the DB');
  mysqli_select_db($this->link,'projekt_it') or die('Cannot select the DB');
 }
 
 function storeInDB($ppm){
  $query = "insert into mq7 set ppm='".$ppm."'";
  $result = mysqli_query($this->link,$query) or die('Errant query:  '.$query);
 }
 
}
if($_GET['ppm'] != ''){
 $mq7=new mq7($_GET['ppm']);
}


?>
