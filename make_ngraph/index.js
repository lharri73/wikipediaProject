var createGraph = require('ngraph.graph');
var save = require('ngraph.tobinary');
var graph = createGraph();

const csv = require('csv-parser');
const fs = require('fs');
var nodeNames = [];

const colorMap = {
            0: [244, 190, 164],
            1: [106, 255, 89 ],
            2: [53 , 179, 252],
            3: [52 , 64 , 235]
        }
// colors = {0: (244,190,164),
//     1: (106,255,89),
//     2: (53,179,252),
//     3: (52,64,235)}

// Simple class used to hold each node's relevant information
class Node{
    constructor(Name, clickLevel){
        this.name = Name;
        this.size = 1;
        this.clickLevel = clickLevel;
    }
    incriment(){
        this.size++;
    }
}

function result(first, second, third, fourth){
    var found = nodeNames.findIndex(element => element.name === first);
    if(found == -1){
        nodeNames.push(new Node(first, 0));
    }else{
        nodeNames[found].incriment();
    }
    found = nodeNames.findIndex(element => element.name === second);
    if(found == -1){
        nodeNames.push(new Node(second, 1));
    }else{
        nodeNames[found].incriment();
    }
    found = nodeNames.findIndex(element => element.name === third);
    if(found == -1){
        nodeNames.push(new Node(third, 2));
    }else{
        nodeNames[found].incriment();
    }
    found = nodeNames.findIndex(element => element.name === fourth);
    if(found == -1){
        nodeNames.push(new Node(fourth, 3));
    }else{
        nodeNames[found].incriment();
    }
    this.first = first
    this.second = second;
    this.third = third;
    this.fourth = fourth;
}

var resultsList = [];

fs.createReadStream('data/Hitler.csv')
  .pipe(csv({headers: false}))
  .on('data', (row) => {
      resultsList.push(new result(row['1'], row['2'], row['3'], row['4']));
  })
  .on('end', () => {
    console.log('CSV file successfully processed');
    for(name in nodeNames){
        console.log("adding nodes");
        graph.addNode(nodeNames[name].name, {
            id: name,
            size: nodeNames[name].size,
            color: colorMap[nodeNames[name].clickLevel],
        });
    }
    console.log(graph.getNodesCount());
    console.log("done");
});
