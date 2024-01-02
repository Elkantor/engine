{
    function connect(elem1, elem2)
    {
        var elem1Rect = getMiddlePosition(elem1);
        var elem2Rect = getMiddlePosition(elem2);
        var x1 = elem1Rect.x;
        var y1 = elem1Rect.y;
        var x2 = elem2Rect.x;
        var y2 = elem2Rect.y;

        var line = document.createElement("div");

        line.setAttribute("connectFrom", elem1.getAttribute("id"))
        line.setAttribute("connectTo", elem2.getAttribute("id"));
        line.style.position = "absolute";
        line.style.borderTop = "1px solid black"; // Utiliser une bordure pour représenter la ligne
        
        var deltaX = x2 - x1;
        var deltaY = y2 - y1;
        
        // Calculer la longueur de la ligne
        var length = Math.sqrt(deltaX * deltaX + deltaY * deltaY);
        
        // Calculer l'angle
        var angle = Math.atan2(deltaY, deltaX);
        
        // Appliquer la longueur à la bordure de la ligne
        line.style.width = length + "px";
        
        // Positionner la ligne en ajustant les coordonnées en fonction de l'angle
        line.style.left = x1 + "px";
        line.style.top = y1 + "px";
        
        // Appliquer la rotation en ajustant les coordonnées x et y
        line.style.transform = "rotate(" + angle + "rad)";
        line.style.transformOrigin = "0 0";
        
        document.body.insertBefore(line, document.body.firstChild);
    }

    function updateConnection(connection)
    {
        var elem1 = document.getElementById(connection.getAttribute("connectto"));
        var elem2 = document.getElementById(connection.getAttribute("connectfrom"));

        var elem1Rect = getMiddlePosition(elem1);
        var elem2Rect = getMiddlePosition(elem2);
        var x1 = elem1Rect.x;
        var y1 = elem1Rect.y;
        var x2 = elem2Rect.x;
        var y2 = elem2Rect.y;

        var deltaX = x2 - x1;
        var deltaY = y2 - y1;
        
        // Calculer la longueur de la ligne
        var length = Math.sqrt(deltaX * deltaX + deltaY * deltaY);
        
        // Calculer l'angle
        var angle = Math.atan2(deltaY, deltaX);
        
        // Appliquer la longueur à la bordure de la ligne
        connection.style.width = length + "px";
        
        // Positionner la ligne en ajustant les coordonnées en fonction de l'angle
        connection.style.left = x1 + "px";
        connection.style.top = y1 + "px";
        
        // Appliquer la rotation en ajustant les coordonnées x et y
        connection.style.transform = "rotate(" + angle + "rad)";
        connection.style.transformOrigin = "0 0";
    }

    function updateConnections(node)
    {
        var paramsQuery = node.querySelectorAll('#params');

        if (paramsQuery.length == 0) return;

        var params = paramsQuery[0].children;

        for (var i = 0; i < params.length; ++i)
        {
            var param1 = params[i];

            var connectionsIn = document.querySelectorAll('[connectfrom=' + param1.getAttribute("id") + ']');
            for (var j = 0; j < connectionsIn.length > 0; ++j)
            {
                var connection = connectionsIn[0];
                updateConnection(connection);
            }
            
            var connectionsOut = document.querySelectorAll('[connectto=' + param1.getAttribute("id") + ']');
            for (var j = 0; j < connectionsOut.length > 0; ++j)
            {
                var connection = connectionsOut[0];
                updateConnection(connection);
            }
        }
        
    }

    function addParam(type, parent, id)
    {
        var rect1 = getMiddlePosition(parent);
        var parentStyle = getComputedStyle(parent);

        var width = 10;
        var height = 10;

        var params = parent.querySelectorAll("#params");
        if (params.length == 0)
        {
            params = document.createElement("div");
            params.setAttribute("id", "params");
            parent.appendChild(params);
        }

        var circle = document.createElement("div");
        circle.setAttribute("id", id);
        circle.style.position = "relative";
        circle.style.borderRadius = "50%";
        circle.style.width = width + "px";
        circle.style.height = height + "px";

        var modifier = (type === "in") ? 0 : 1;
        circle.style.left = (modifier * (parseInt(parentStyle.width))) - (width / 2) + "px";
        circle.style.top = parseInt(parentStyle.height)/2 - height/2 + "px";

        circle.style.backgroundColor = "white"; // Couleur du cercle (vous pouvez ajuster selon vos besoins)
        circle.style.border = "1px solid rgb(139, 139, 139)";
        params.appendChild(circle);
        return circle;
    }

    function getMiddlePosition(element) {
        var rect = element.getBoundingClientRect();
        var middleX = rect.left + rect.width / 2;
        var middleY = rect.top + rect.height / 2;
        return { x: middleX, y: middleY };
    }
      
    document.addEventListener("mousedown", function (e) {
        if (e.target.classList.contains("draggable")) {
            isDragging = true;
            draggedElement = e.target;

            offsetX = e.clientX - draggedElement.getBoundingClientRect().left;
            offsetY = e.clientY - draggedElement.getBoundingClientRect().top;

            document.addEventListener("mousemove", handleMouseMove);
            document.addEventListener("mouseup", function () {
                isDragging = false;
                draggedElement = null;
                document.removeEventListener("mousemove", handleMouseMove);
            });
        }
    });

    function handleMouseMove(e) {
        if (isDragging && draggedElement) 
        {
            draggedElement.style.left = e.clientX - offsetX + "px";
            draggedElement.style.top = e.clientY - offsetY + "px";

            updateConnections(draggedElement);
        }
    }

    // Exemple d'utilisation
    var element1 = document.getElementById("div1");
    var element2 = document.getElementById("div2");

    var param1 = addParam("out", element1, "out1");
    var param2 = addParam("in", element2, "in1");
    connect(param1, param2);
}