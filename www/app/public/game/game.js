var sequence = [];
var pressedCount = 0;
var keyPress = true;

function newSequence()
{
	var random = Math.floor(Math.random() * 4) + 1;
	return (getColor(random));
}

function getColor(key)
{
	switch (key) {
		case 1:
			return ("green");
		case 2:
			return ("red");
        case 3:
            return ("yellow");				
		case 4:
			return ("blue");
		default:
			return ("wrong");
	}
}

function playSound(key)
{
	var soundPath = "/game/sounds/" + key + ".mp3";
	var audio = new Audio(soundPath);
	audio.play();
}

function buttonAnimation(key)
{
    var activeButton = document.getElementById(key);
    activeButton.classList.add("pressed");
    setTimeout(function () {
        activeButton.classList.remove("pressed")
    }, 100);
}
function gameOver() 
{
	playSound("wrong");
	document.querySelector("body").style.background = "red";
	setTimeout(function () {
		document.querySelector("body").style.background = "#CDE8E5";
    }, 100);
	sequence = [];
	document.getElementById("level-title").innerHTML = "Game Over, Press Space to Restart";
	keyPress = true;


}

for (var i = 0; i < document.querySelectorAll(".btn").length; i++)
{
	document.querySelectorAll(".btn")[i].addEventListener("click", function() 
	{
		var key = this.id;
		if (sequence[pressedCount] === key)
		{	
			playSound(key);
			pressedCount++;
			if (pressedCount === sequence.length)
			{
				setTimeout(function() {
				runSequence();}, 1000);
				}
		}
		else
			gameOver();			
	});
}

function runSequence()
{
	var newKey = newSequence();
	sequence.push(newKey);
	pressedCount = 0;
	playSound(newKey);
	buttonAnimation(newKey);
	document.getElementById("level-title").innerHTML = "Level " + sequence.length;
}
document.addEventListener("keydown", function(event) {
	if (keyPress && event.key === ' ')
	{
		keyPress = false;
		setTimeout(function() {
			runSequence();}, 200);
	}
});

/* 
+++++OPTION TODO'S
- safe highest level in database
- authentication 
- tournament system 
- info button about the rules
- playing on smartphone!
*/