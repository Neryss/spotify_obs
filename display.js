/*
	outputs a defiling song name as text
	You can use it for OBS for example, so song names don't take 90% of your stream while still being readable
	It takes a file path where the song name is stored (you can use this OBS plugin https://obsproject.com/forum/resources/obscurrentsong-spotify.658/)
	and crop it to fit a 40 characters string written to an output file
*/

const fs = require("fs");
var	char_limit = 40;
var	current;
var	first = true;
var	has_changed = false;
//	read song name from file
var	readPath = "./out.txt";
//	write the result inside another file
var	writePath = "./display.txt";

function animateString(path) {
	console.log("Animate");
	try
	{
		data = fs.readFileSync(path)
		console.log(data.toString());
		if (current != data.toString())
			str = data.toString();
		else
			str = current;
		setInterval(function () {
			data = fs.readFileSync(path)
			if (!data.toString())
			{
				fs.writeFileSync(writePath, "");
				return ;
			}
			if (current != data.toString())
			{
				str = data.toString();
				current = str.toString();
				has_changed = true;
			}
			console.log(str.length);
			if (!first && !has_changed && str.length > char_limit)
			{
				var tmp = str.toString()[0];
				str = str.toString().substring(1, str.length) + tmp;
			}
			else if ((first || has_changed) && str.length > char_limit)
			{
				has_changed = false;
				str = str.toString() + " | ";
			}
			console.log(str.toString());
			if (str.length > char_limit)
			{
				fs.writeFileSync(writePath, "♫ " + str.toString().substring(0, char_limit));
			}
			else
				fs.writeFileSync(writePath, "♫ " + str.toString());
			first = false;
		}, 1000);
	} catch (err) {
		console.error(err);
	}
}

animateString(readPath);