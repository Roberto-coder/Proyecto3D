

function assert(a) {
    if (!a) console.log("Assertion failed in bresenham.js "+a);
    return a;
 }
    
 function plotLine(x0, y0, x1, y1)
 {
    var dx =  Math.abs(x1-x0), sx = x0<x1 ? 1 : -1;
    var dy = -Math.abs(y1-y0), sy = y0<y1 ? 1 : -1;
    var err = dx+dy, e2;                                   /* error value e_xy */
 
    for (;;){                                                          /* loop */
       setPixel(x0,y0);
       if (x0 == x1 && y0 == y1) break;
       e2 = 2*err;
       if (e2 >= dy) { err += dy; x0 += sx; }                        /* x step */
       if (e2 <= dx) { err += dx; y0 += sy; }                        /* y step */
    }
 }
 
 function plotEllipse(xc, yc, a, b){
   let x = 0;
   let y = b;
   let d1 = b * b - a * a * b + 0.25 * a * a;

   // Primera región pendiente es menor a 1 (es decir, dx < dy)
   while (2 * b * b * x < 2 * a * a * y) {
       setPixel(xc + x, yc + y);
       setPixel(xc - x, yc + y);
       setPixel(xc + x, yc - y);
       setPixel(xc - x, yc - y);
       
       if (d1 < 0) {
           x++;
           d1 = d1 + 2 * b * b * x + b * b;
       } else {
           x++;
           y--;
           d1 = d1 + 2 * b * b * x - 2 * a * a * y + b * b;
       }
   }

   // Segunda región dx >= dy
   let d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;
   while (y >= 0) {
       setPixel(xc + x, yc + y);
       setPixel(xc - x, yc + y);
       setPixel(xc + x, yc - y);
       setPixel(xc - x, yc - y);

       if (d2 > 0) {
           y--;
           d2 = d2 - 2 * a * a * y + a * a;
       } else {
           y--;
           x++;
           d2 = d2 + 2 * b * b * x - 2 * a * a * y + a * a;
       }
   }
 }
 
 function plotCircle(xc, yc, r){
   let x = 0;
   let y = r;
   let d = 3 - 2 * r;

   while (y >= x) {
       // Trazar los puntos en los 8 octantes
       setPixel(xc + x, yc + y);
       setPixel(xc - x, yc + y);
       setPixel(xc + x, yc - y);
       setPixel(xc - x, yc - y);
       setPixel(xc + y, yc + x);
       setPixel(xc - y, yc + x);
       setPixel(xc + y, yc - x);
       setPixel(xc - y, yc - x);

       x++;
       if (d < 0) {
           d = d + 4 * x + 6;
       } else {
           y--;
           d = d + 4 * (x - y) + 10;
       }
   }
 }
 

 function setPixel(x, y, color = 'black', opacity = 1.0) {
   ctx.fillStyle = color;
   ctx.globalAlpha = opacity;
   ctx.fillRect(x, y, 1, 1); // Dibuja un píxel de 1x1 en la posición (x, y)
   ctx.globalAlpha = 1.0;    // Restablece la opacidad a 1.0 para futuras operaciones
}





