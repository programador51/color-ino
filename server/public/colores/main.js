/* Se exporta la libreria para el front-end de la app
UTILIZANDO UN BUNDLER COMO BROWSERIFY O BABELIFY */
const simpleColorConverter = require('simple-color-converter');

const sockets = io();
let i;
let comas = [];
let tonalidad = 0;
let saturacion = 0;
let luminancia = 0;
let tendencia;
let cajaColor = document.getElementById('caja-color');
let botonDOM = document.getElementById('detenerSensado');
let sistemaDOM = document.getElementById('obtenerSistema');
let sliderDOMTono = document.getElementById('sliderTono');
let sliderDOMSaturacion = document.getElementById('sliderSaturacion');
let sliderDOMLuminosidad = document.getElementById('sliderLuminosidad');
let outputPantallaCMYK = document.getElementById('outputPantallaCMYK');
let pantallaCMYK = document.getElementById('pantallaCMYK');
let codigoCMYK = document.getElementById('codigoCMYK');
let pantoneDOM = document.getElementById('pantallaCMYK');

let leerColor = true;

let sliders = document.querySelectorAll("input");

for(i=0;i<sliders.length;i++){
    sliders[i].addEventListener("input",()=>{
        cajaColor.style.backgroundColor = `
        hsl(${tonalidad},${saturacion}%,${luminancia}%)`;
        console.log('Leyendo slider...');
    })
}

function detenerSensado(){
    leerColor = !leerColor;
    console.log(`leerColor = ${leerColor}`);
    leerColor === false ? ajustarSliders() : null
    leerColor === false ? botonDOM.innerHTML = 'Iniciar sensado' : botonDOM.innerHTML = 'Detener sensado'
    leerColor === false ? sistemaDOM.disabled = false : sistemaDOM.disabled = true
}

function ajustarSliders(){
    sliderDOMTono.value = tonalidad;
    sliderDOMSaturacion.value = saturacion;
    sliderDOMLuminosidad.value = luminancia;
}
    sockets.on('colores',(data)=>{
    if(leerColor === true){
    document.getElementById('detenerSensado').
    addEventListener("click",detenerSensado);

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

    let color = new simpleColorConverter({
        hsl: {h:tonalidad,s:saturacion,l:luminancia}, 
        to: 'cmyk'
    });
    
    //console.log(color.color);
    
    /* Se hace vacía para no guardar las posiciones de las iteraciones anteriores */
    comas = [];
    }else{
        let cmyk = new simpleColorConverter({
            hsl: {h:sliderDOMTono.value,
                s:sliderDOMSaturacion.value,
                l:sliderDOMLuminosidad.value}, 
            to: 'cmyk'
        });

        console.log(`CMYK IMPRESION : ${JSON.stringify(cmyk.color)}`);

        /* PARA IMPRIMIR EL COLOR EN LA PANTALLA */
        let pantallaCMYK = new simpleColorConverter({
            cmyk: {c:cmyk.color.c,
                m:cmyk.color.m,
                y:cmyk.color.y,
                k:cmyk.color.k
                }, 
            to: 'rgb'
        });
        
        outputPantallaCMYK.innerHTML = `
        rgb(${pantallaCMYK.color.r},${pantallaCMYK.color.g},${pantallaCMYK.color.b})`;

        codigoCMYK.innerHTML = `
        C : ${cmyk.color.c} | M : ${cmyk.color.m} | Y: ${cmyk.color.y} | K : ${cmyk.color.k}
        `;

        //pantallaCMYK.style.backgroundColor = `${outputPantallaCMYK.textContent}`;

        console.log(`CMYK PANTALLA : ${JSON.stringify(pantallaCMYK.color)}`);
        
        let pantone = new simpleColorConverter({
            hsl: {h:sliderDOMTono.value,
                s:sliderDOMSaturacion.value,
                l:sliderDOMLuminosidad.value}, 
            to: 'pantone'
        });
        
        pantoneDOM.innerHTML = `PANTONE ${pantone.color}`;
        console.log(`PANTONE IMPRESION : ${pantone.color}`);
    }
});