// document.body.style.backgroundColor = "red";

document.getElementById('rotateButton').addEventListener('click', function() {
    var rotateContainer = document.querySelector('.rotate-container');
    if (!rotateContainer.classList.contains('rotating')) {
        rotateContainer.classList.add('rotating');
    } else {
        rotateContainer.classList.remove('rotating');
    }
});

document.getElementById('spinButton').addEventListener('click', function() {
    const wheel = document.getElementById('wheel');
    const maxAngle = Math.random() * 360; // Random angle between 0 and 360 degrees
    const duration = 5000; // Duration of the spin in milliseconds

    // Animate the wheel to the calculated angle
    wheel.style.transition = `${duration}ms linear`;
    wheel.style.transform = `rotate(${maxAngle}deg)`;
});
