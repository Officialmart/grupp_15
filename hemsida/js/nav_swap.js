/*------------------------------------------------------------------------- 
Denna fil (nav_swap.js) används för att bestämma vilken div som ska
användas i HTML koderna beroende på användarens skärmen storlek.
Använder jquery 3.3.1
-------------------------------------------------------------------------*/
(function ($) {
    $(document).ready(function () { //När sidan är redo så körs koderna inom denna funktion
        if ($(window).width() < 600) { //Om bredden på skärmen är mindre än 600 px (mobiltelefon eller annat)...
            $("#nav_div_mid").fadeOut(0); //... Göm denna div, detta utförs på 0 ms
            $("#nav_div_right").fadeOut(0); //... Göm denna div, detta utförs på 0 ms
         }
         else{ //Om bredden på skärmen är större än 600 px (stationära dator, laptops eller annat)...
            $("#nav_div_right_alt").fadeOut(0); //.. Göm denna div, detta utförs på 0 ms
            $("#nav_div_mid_alt").fadeOut(0); //.. Göm denna div, detta utförs på 0 ms
         }
    });
  })(jQuery);