const sockets = io();
let i;
let comas = [];
let tonalidad;
let saturacion;
let luminancia;
let tendencia;

//console.log(color);

sockets.on('colores',(data)=>{
    let valores
    valores = data;
    //document.getElementById('vals-color').innerHTML = data;

    /* Se detecta la posicion de las comas y se guardan dichas posiciones
    Para despues recorrer el dato que nos da el arduino (Una linea string separada por comas)
    y guardarlo en variables de JS */
    for(i=0;i<valores.length;i++){
        if(valores.charAt(i)==',')
        comas.push(i);
    }

    tonalidad = parseFloat(valores.substring(0,comas[0]));
    saturacion = parseFloat(valores.substring(comas[0]+1,comas[1]));
    luminancia = parseFloat(valores.substring(comas[1]+1,comas[2]));
    tendencia = valores.substring(comas[2]+1,valores.length);

    //console.log(`Tonalidad = ${tonalidad}\nSaturacion = ${saturacion}\nLuminancia = ${luminancia}`);

    document.getElementById('tono').innerHTML = tonalidad;
    document.getElementById('saturacion').innerHTML = saturacion;
    document.getElementById('luminancia').innerHTML = luminancia;
    document.getElementById('tendencia').innerHTML = tendencia;

    let estilo = `hsl(${tonalidad},${saturacion}%,${luminancia}%)`;
    //console.log(estilo);

    if(tendencia=='blanco'){
        estilo = `hsl(0,0%,100%)`;
    }

    document.getElementById('caja-color').style.backgroundColor = estilo;
   
    /* Se hace vacía para no guardar las posiciones de las iteraciones anteriores */
    comas = [];
});