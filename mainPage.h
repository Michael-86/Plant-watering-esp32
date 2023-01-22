const char MAIN_page[] PROGMEM = R"=====(
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<HTML>
<TITLE>
Automatisk växt bevatning och monitoring
</TITLE>
<BODY>
<CENTER>
<FORM method="post" action="/form">
<TABLE>
<TR><TD colspan=2><B>Mickes Auto plant monitoring center.</B></TD></TR>

<TR><TD><div>Tempratur : <span id="tempratur">0</span> C<br></div></TD>
<TR><TD><div>Luftfuktighet : <span id="luftfuktighet">0</span> %<br></div></TD>
<TR><TD><div>Ljusstyrka : <span id="ljusstyrka">0</span> <br></div></TD>
<TR><TD><div>Jordfuktighet : <span id="jordfuktighet">0</span> <br></div></TD>
<TR><TD><div><span></span> <br></div></TD><TD>Load Status</TD>

<TR><TD>
<p>För automatisk eller manuell körning.</p>
<TR><TD>
<button type="button" onclick="sendData1(1)">ON</button>
<button type="button" onclick="sendData1(0)">OFF</button><BR>
</TD>
<TD><span id="state1">NA</span></TD></TR>
<TR><TD>
<p>Starta Pumpen manuellt.</p>
<TR><TD>
<button type="button" onclick="sendData2(1)">ON</button>
<button type="button" onclick="sendData2(0)">OFF</button><BR>
</TD>
<TD><span id="state2">NA</span></TD></TR>
</div>
<script>


function sendData1(led) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState1").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "button1?LEDstate1="+led, true);
  xhttp.send();
}

function sendData2(led) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState2").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "button2?LEDstate2="+led, true);
  xhttp.send();
}

//------------------------------------------------------------

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData1();
  getData2();
  getData3();
  getData4();
  getData5();
  getData6();
}, 2000); //2000mSeconds update rate
 
function getData1() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("tempratur").innerHTML =
      this.responseText;    
    }
  };
  xhttp.open("GET", "readtemp", true);
  xhttp.send();
}

function getData2() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("luftfuktighet").innerHTML =
      this.responseText;      
    }
  };
  xhttp.open("GET", "readhum", true);
  xhttp.send();
}

function getData3() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ljusstyrka").innerHTML =
      this.responseText;      
    }
  };
  xhttp.open("GET", "readljus", true);
  xhttp.send();
}

function getData4() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("jordfuktighet").innerHTML =
      this.responseText;      
    }
  };
  xhttp.open("GET", "readjordfuktighet", true);
  xhttp.send();
}

function getData5() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state1").innerHTML =
      this.responseText;      
    }
  };
  xhttp.open("GET", "button1state", true);
  xhttp.send();
}

function getData6() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state2").innerHTML =
      this.responseText;      
    }
  };
  xhttp.open("GET", "button2state", true);
  xhttp.send();
}

</script>

<TR><TD colspan=2><B><CENTER><A href = "/ota">Update</a></CENTER></B></TD></TR>
</TABLE>
</FORM>

</CENTER>
</BODY>
</HTML>
)=====";
