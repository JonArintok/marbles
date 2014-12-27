
outType eval_currentFrame(_evalargs_) {
	outType toBeReturned;
	toBeReturned.n = curFrame;
	return toBeReturned;
}
const stdNode node_curFrame = {
	.name = "currentFrame N1",
	.arity = 0,
	.evaluate = eval_currentFrame
};


outType eval_frameRate(_evalargs_) {
	outType toBeReturned;
	toBeReturned.n = frameRate;
	return toBeReturned;
}
const stdNode node_frameRate = {
	.name = "frameRate N1",
	.arity = 0,
	.evaluate = eval_frameRate
};



outType eval_videoWidth(_evalargs_) {
	outType toBeReturned;
	toBeReturned.n = videoWidth;
	return toBeReturned;
}
const stdNode node_videoWidth = {
	.name = "videoWidth N1",
	.arity = 0,
	.evaluate = eval_videoWidth
};

outType eval_videoHeight(_evalargs_) {
	outType toBeReturned;
	toBeReturned.n = videoHeight;
	return toBeReturned;
}
const stdNode node_videoHeight = {
	.name = "videoHeight N1",
	.arity = 0,
	.evaluate = eval_videoHeight
};



outType eval_windowWidth(_evalargs_) {
	outType toBeReturned;
	toBeReturned.n = windowWidth;
	return toBeReturned;
}
const stdNode node_windowWidth = {
	.name = "windowWidth N1",
	.arity = 0,
	.evaluate = eval_windowWidth
};

outType eval_windowHeight(_evalargs_) {
	outType toBeReturned;
	toBeReturned.n = windowHeight;
	return toBeReturned;
}
const stdNode node_windowHeight = {
	.name = "windowHeight N1",
	.arity = 0,
	.evaluate = eval_windowHeight
};
