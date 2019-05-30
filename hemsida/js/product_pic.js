/*------------------------------------------------------------------------- 
Denna fil (product_pic.js) används för att skapa animering när sidan
laddats färdigt. En div hissas ned och en bild samt text till bilden
tonas in långsamt.
Använder jquery 3.3.1
-------------------------------------------------------------------------*/
(function ($) {
    $(document).ready(function () { //När sidan är redo ska koderna i denna funktion köras
        $("#content_product_img").slideUp(0); //Hissa upp denna div på 0 ms
        $("#product_img").fadeOut(0); //Tona ut denna div
        $("#product_text").fadeOut(0); //Tona ut denna div
        $("#content_product_buy_gallery").fadeOut(0); //Tona ut denna div
        $("#content_product_img").slideDown(1000); //Hissa ned denna div
        setTimeout(function () { //En delay har satts på en sekund för koderna i denna funktion
            $("#product_img").fadeIn(); //Tona in denna div efter en sekund
            setTimeout(function () { //Efter ytterligare en sekund...
                $("#product_text").fadeIn();  //...Tona in denna div
                $("#content_product_buy_gallery").fadeIn(); //...Tona ut denna div
            }, 1000);
        }, 1000);
    });
})(jQuery);


