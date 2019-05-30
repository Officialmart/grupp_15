/*------------------------------------------------------------------------- 
Denna fil (nav.js) används för att ändra den nedre navbaren som innehåller
länkar. Om användaren skrollar ned mer än 150 px från toppen så ändras
navbaren till en fast navbar, som förljer med användaren så länge användaren
befinner sig mer än 150 px nedåt från toppen.
Använder jquery 3.3.1
-------------------------------------------------------------------------*/
let i = 0;
let j = 0;
(function ($) {
  $(document).ready(function () { //När sidan är redo körs koderna innanför funktionen
    $("#nav2_mid_alt").fadeOut(0); //Göm diven nav2_mid_alt
    $(window).scroll(function () { //Denna funktion körs varje gång användaren skrollar
      if ($(this).scrollTop() > 150) { //Om användaren skrollat mer än 150 px nedåt från toppen...
        $("#nav2_mid").fadeOut(0); //...Göm denna div
        $('.nav2').addClass('nav2_alt'); //... lägg till alla egenskaper från klassen nav2_alt till nav2  
        $('#nav2_mid_alt').fadeIn(200); //Visa denna div för användaren
      } else { //Om användaren inte skrollat mer än 150 px från toppen...
        $("#nav2_mid_alt").fadeOut(0); //... Göm denna div
        $('.nav2').removeClass('nav2_alt'); //... Ta bort egenskaperna som nav2 fick från nav2_alt
        $('#nav2_mid').fadeIn(200); //Visa denna dic för användaren
      }
    });
  });
})(jQuery);