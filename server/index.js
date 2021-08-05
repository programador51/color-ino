const Serialport = require('serialport');
const express = require('express');
const http = require('http');
const socketIO = require('socket.io');

const app = express();
server = http.createServer(app);
const io = socketIO.listen(server);

app.use(express.static(__dirname+'/public'));

server.listen(3000,()=>{
    console.log('Servidor en puerto ',3000);
});



// SERIAL

/* Con esto podemos leer la informacion que nos arroje el arduino
de una forma entendible para los humanos */
const Readline = Serialport.parsers.Readline;

/* Se configura el puerto del arduino*/
const port = new Serialport('COM5',{
    baudRate: 115200
});

const parser = port.pipe(new Readline({
    delimiter:'\r\n'
}));

parser.on('open',()=>{
    console.log('Conexion abierrta');

});

parser.on('data',(data)=>{
    //console.log(parseFloat(data,10));
    console.log(data);
    io.emit('colores',data);
});

parser.on('error',(e)=>{
    console.log(e)
})