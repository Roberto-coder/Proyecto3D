const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
let vertices = [];

// Función para dibujar el contorno del polígono
function drawPolygon() {
    if (vertices.length < 2) return;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.beginPath();
    ctx.moveTo(vertices[0][0], vertices[0][1]);
    for (let i = 1; i < vertices.length; i++) {
        ctx.lineTo(vertices[i][0], vertices[i][1]);
    }
    ctx.closePath();
    ctx.stroke();
}

function fillPolygonAnimated(color) {
    if (vertices.length < 3) return; // Se necesita al menos un polígono

    // Encuentra los límites del polígono
    let minY = vertices[0][1], maxY = vertices[0][1];
    for (let i = 1; i < vertices.length; i++) {
        minY = Math.min(minY, vertices[i][1]);
        maxY = Math.max(maxY, vertices[i][1]);
    }

    let y = minY; // Empieza desde el límite superior del polígono

    // Función que realiza el relleno de cada scanline de forma progresiva
    function fillScanline() {
        if (y > maxY) return; // Detiene la animación cuando se alcanza el límite inferior

        let intersections = []; // Guarda las intersecciones en la línea actual (scanline)

        // Encuentra las intersecciones con los bordes del polígono en la línea actual (y)
        for (let i = 0; i < vertices.length; i++) {
            let start = vertices[i];
            let end = vertices[(i + 1) % vertices.length];

            if ((start[1] <= y && end[1] > y) || (end[1] <= y && start[1] > y)) {
                // Calcula la intersección en x
                let x = start[0] + (y - start[1]) * (end[0] - start[0]) / (end[1] - start[1]);
                intersections.push(x);
            }
        }

        intersections.sort((a, b) => a - b); // Ordena las intersecciones de izquierda a derecha

        // Dibuja líneas horizontales entre pares de intersecciones
        for (let i = 0; i < intersections.length; i += 2) {
            if (i + 1 < intersections.length) {
                ctx.beginPath();
                ctx.moveTo(intersections[i], y);
                ctx.lineTo(intersections[i + 1], y);
                ctx.strokeStyle = color;
                ctx.stroke();
            }
        }

        y++; // Incrementa `y` para ir a la siguiente línea horizontal
        setTimeout(fillScanline, 10); // Llama a la función de nuevo con un retraso de 10 ms
    }

    fillScanline(); // Inicia la animación de barrido
}


// Función para manejar los clicks y registrar los vértices
canvas.addEventListener('click', (event) => {
    const rect = canvas.getBoundingClientRect();
    const x = event.clientX - rect.left;
    const y = event.clientY - rect.top;

    // Guardar el punto en el arreglo de vértices
    vertices.push([x, y]);
    drawPolygon();
});

// Función para limpiar el canvas y reiniciar los vértices
function clearCanvas() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    vertices = [];
}
