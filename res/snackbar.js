function snackbar(text) 
{
    var x = document.getElementById("snackbar");

    if (x.className == "show")
    {
        return;
    }

    x.className = "show";
    x.innerText = text;
    setTimeout(function(){ x.className = x.className.replace("show", ""); }, 3000);
}