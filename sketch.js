
const controls = {
	view: {x: 0, y: 0, zoom: 1},
	viewPos: { prevX: null,  prevY: null,  isDragging: false },
  }
var frameCount = 0;
var evalCount = 10;
var visible = [];
function setup() {
	createCanvas(windowWidth, windowHeight);
	background(0)
	nodes = []
	for(let i = 0; i < 3000; i++){
		nodes.push(new Node("test", 0, 60, 30*i, 0));
	}
	for(node of nodes){
		node.draw()
	}
	visible = nodes;
}
function draw() {
	console.log("numVisible", visible.length)
	console.log(controls.view);
	console.log(nodes[0].position.x);
	console.log(width, windowWidth);
	background(0);
	translate(controls.view.x, controls.view.y);
	scale(controls.view.zoom)
	if(+frameCount >= evalCount){
		visible = [];
		for(node of nodes){
			if(node.eval_visible()){
				visible.push(node);
				node.draw();
			}
		}
		frameCount = 0;
	}else{
		for(node of visible){
			node.draw();
		}
	}

}

class Node {
	constructor(name, clickLevel, posX, posY, numConnections){
		const colors = {0: (244,190,164),
			1: (106,255,89),
			2: (53,179,252),
			3: (52,64,235)}

		const clickLevelSize = {0: 30,
								1: 50,
								2: 70,
								3: 90}
		
		this.name = name;
		this.clickLevel = clickLevel;
		this.position = createVector(posX, posY);
		this.numConnections = numConnections;

		this.radius = colors[clickLevel] + numConnections;
		this.color = colors[clickLevel];
	}
	draw(){
		fill(this.color)
		ellipse(this.position.x, this.position.y, this.radius);

	}
	eval_visible(){
		var insideX = this.position.x < -controls.view.x + windowWidth && this.position.x > controls.view.x;
		var insideY = true;//this.position.y < controls.view.y + windowHeight/2 && this.position.y > controls.view.y - windowHeight/2;
		if(insideX && insideY){
			return(true);
		}else{
			return(false);
		}
	}
}

function mouseWheel(e){
	Controls.zoom(controls).worldZoom(e)
}

function mousePressed(e){
	Controls.move(controls).mousePressed(e);
}

function mouseDragged(e){
	Controls.move(controls).mouseDragged(e);
}

function mouseReleased(e){
	Controls.move(controls).mouseReleased(e);
}
