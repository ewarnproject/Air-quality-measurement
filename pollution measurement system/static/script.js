const map = L.map('map').setView([22.2604, 84.8536], 12);

L.tileLayer(
'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
{
attribution:'OpenStreetMap'
}
).addTo(map);

L.marker([22.2604,84.8536])
.addTo(map)
.bindPopup("NIT Rourkela AQI Monitoring Station")
.openPopup();

const gaugeChart = new Chart(
document.getElementById('gaugeChart'),
{
type:'doughnut',

data:{
labels:['AQI','Remaining'],
datasets:[{
data:[0,500],
backgroundColor:[
'#00c853',
'#e0e0e0'
],
borderWidth:0
}]
},

options:{
responsive:true,
maintainAspectRatio:false,
cutout:'75%',
rotation:-90,
circumference:180,
plugins:{
legend:{
display:false
}
}
}
}
);

const pollutantChart = new Chart(
document.getElementById('pollutantChart'),
{
type:'line',

data:{
labels:[],
datasets:[
{
label:'MQ2 Smoke',
data:[],
borderColor:'blue',
fill:false,
tension:0.3
},
{
label:'MQ3 Alcohol',
data:[],
borderColor:'orange',
fill:false,
tension:0.3
},
{
label:'MQ135 Air Quality',
data:[],
borderColor:'red',
fill:false,
tension:0.3
}
]
},

options:{
responsive:true,
maintainAspectRatio:false,

plugins:{
legend:{
display:true,

onClick:function(e, legendItem, legend){

const clickedIndex =
legendItem.datasetIndex;

const chart =
legend.chart;

chart.data.datasets.forEach(
(dataset,index)=>{

if(index===clickedIndex){

dataset.hidden=false;

}
else{

dataset.hidden=true;

}

});

chart.update();

}
}
},

scales:{
y:{
beginAtZero:true
}
}
}
}
);

let mq2History=[];
let mq3History=[];
let mq135History=[];

function getAvg(arr){

if(arr.length===0)
return 0;

let total=0;

for(let i=0;i<arr.length;i++){

total+=arr[i];

}

return Math.round(total/arr.length);

}

function updateDateTime(){

const now=new Date();

const dateTime =
now.toLocaleDateString()+
" | "+
now.toLocaleTimeString();

if(document.getElementById("datetime")){

document.getElementById("datetime").innerHTML =
dateTime;

}

}

function updateDashboard(){

fetch('/latest')

.then(response=>response.json())

.then(data=>{

const mq2 = data.mq2 || 0;
const mq3 = data.mq3 || 0;
const mq135 = data.mq135 || 0;
const aqi = data.aqi || 0;
const status = data.status || "Waiting";

document.getElementById("aqiValue").innerHTML =
aqi;

document.getElementById("status").innerHTML =
status;

if(document.getElementById("lastUpdated")){

document.getElementById("lastUpdated").innerHTML =
new Date().toLocaleTimeString();

}

let color="#00c853";
let message="";

if(aqi<=50){

color="#00c853";
message="Air quality is excellent and poses little or no risk.";

}
else if(aqi<=100){

color="#8bc34a";
message="Air quality is acceptable for most people.";

}
else if(aqi<=200){

color="#ff9800";
message="Sensitive people may experience health effects.";

}
else if(aqi<=300){

color="#f44336";
message="Health effects may be experienced by everyone.";

}
else if(aqi<=400){

color="#9c27b0";
message="Very poor air quality. Avoid outdoor exposure.";

}
else{

color="#000000";
message="Severe pollution. Health emergency conditions.";

}

document.getElementById("status").style.background =
color;

if(document.getElementById("aqiMessage")){

document.getElementById("aqiMessage").innerHTML =
message;

}

gaugeChart.data.datasets[0].backgroundColor[0] =
color;

gaugeChart.data.datasets[0].data = [
aqi,
500-aqi
];

gaugeChart.update();

const currentTime =
new Date().toLocaleTimeString();

pollutantChart.data.labels.push(
currentTime
);

pollutantChart.data.datasets[0].data.push(mq2);
pollutantChart.data.datasets[1].data.push(mq3);
pollutantChart.data.datasets[2].data.push(mq135);

if(
pollutantChart.data.labels.length > 20
){

pollutantChart.data.labels.shift();

pollutantChart.data.datasets[0].data.shift();
pollutantChart.data.datasets[1].data.shift();
pollutantChart.data.datasets[2].data.shift();

}

pollutantChart.update();

mq2History.push(mq2);
mq3History.push(mq3);
mq135History.push(mq135);

document.getElementById("mq2min").innerHTML =
Math.min(...mq2History);

document.getElementById("mq2max").innerHTML =
Math.max(...mq2History);

document.getElementById("mq2avg").innerHTML =
getAvg(mq2History);

document.getElementById("mq3min").innerHTML =
Math.min(...mq3History);

document.getElementById("mq3max").innerHTML =
Math.max(...mq3History);

document.getElementById("mq3avg").innerHTML =
getAvg(mq3History);

document.getElementById("mq135min").innerHTML =
Math.min(...mq135History);

document.getElementById("mq135max").innerHTML =
Math.max(...mq135History);

document.getElementById("mq135avg").innerHTML =
getAvg(mq135History);

})

.catch(error=>{

console.log(
"Server Error:",
error
);

});

}

updateDateTime();
setInterval(updateDateTime,1000);

updateDashboard();
setInterval(updateDashboard,3000);

document.getElementById("pollutantChart")
.addEventListener("dblclick",function(){

pollutantChart.data.datasets.forEach(
dataset=>{

dataset.hidden=false;

});

pollutantChart.update();

});