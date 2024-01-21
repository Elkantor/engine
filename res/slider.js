{
    function moveSlider(sliderThumb, sliderTrack, clientX) {
        const trackRect = sliderTrack.getBoundingClientRect();
        const newPosition = Math.max(0, Math.min(clientX - trackRect.left, trackRect.width));
        sliderThumb.style.left = newPosition - sliderThumb.offsetWidth / 2 + 'px';

        const totalWidth = trackRect.width;
        const percentage = (newPosition / totalWidth) * 100;

        const percentageDisplay = document.getElementById('percentageDisplay');
        percentageDisplay.innerText = percentage.toFixed(2) + '%';

        document.getElementById("updateCameraX").click();
    }

    function handleMouseDown(sliderThumb, sliderTrack, e) {
        document.onmousemove = function (e) {
            moveSlider(sliderThumb, sliderTrack, e.clientX);
        };

        document.onmouseup = function () {
            document.onmousemove = null;
            document.onmouseup = null;
        };
    }

    function handleTrackClick(sliderThumb, sliderTrack, e) {
        moveSlider(sliderThumb, sliderTrack, e.clientX);
    }

    const sliderThumb = document.getElementById('sliderThumb');
    const sliderTrack = document.getElementById('sliderTrack');

    sliderThumb.onmousedown = function (e) {
        handleMouseDown(sliderThumb, sliderTrack, e);
    };

    sliderTrack.onclick = function (e) {
        handleTrackClick(sliderThumb, sliderTrack, e);
    };

    function updateSlider(value)
    {
        document.getElementById("percentageDisplay").innerText = parseFloat(value).toFixed(3);
        return true;
    }
}