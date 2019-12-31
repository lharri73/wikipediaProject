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
console.log("reading csv file");

fs.createReadStream('data/Hitler.csv')
  .pipe(csv({headers: false}))
  .on('data', (row) => {
      resultsList.push(new result(row['1'], row['2'], row['3'], row['4']));
  })
  .on('end', () => {
    console.log('CSV file successfully processed');
    console.log("adding nodes");
    for(name in nodeNames){
        graph.addNode(nodeNames[name].name, {
            id: name,
            size: nodeNames[name].size,
            clickLevel: nodeNames[name].clickLevel,
        });
    }
    console.log(graph.getNodesCount(), "Nodes processed");
    console.log("Adding Links");
    
    for(result of resultsList){
        if(result.second == "") continue; //we randomly clicked on hitler...Interesting...?
        else if(result.third == ""){
            // it took only one click to get there
            graph.addLink(result.first, result.second);
        }else if(result.fourth == ""){
            // it took only 2 clicks to get there
            graph.addLink(result.first, result.second);
            graph.addLink(result.second, result.third);
        }else{
            // all fields are populated
            graph.addLink(result.first, result.second);
            graph.addLink(result.second, result.third);
            graph.addLink(result.third, result.fourth);
        }
    }
    console.log("...done");

    // We must write the click levels and size by hand because ngraph.tobinary won't do it for us
    console.log("saving");
    sizes = [];
    clickLevels = [];
    graph.forEachNode(saveSize);
    function saveSize(node){
        sizes.push(node.data.size);
        clickLevels.push(node.data.clickLevel);
    }
    
    fs.writeFileSync('data/sizes.json', JSON.stringify(sizes), 'utf8');
    fs.writeFileSync('data/clickLevels.json', JSON.stringify(clickLevels), 'utf8');
    // the rest can be handled by ngraph.tobinary
    save(graph, {
        outDir: 'data', // folder where to save results. '.' by default
        labels: 'labels.json', // name of the labels file. labels.json by default
        meta: 'meta.json', // name of the file with meta information. meta.json by default
        links: 'links.bin' // file name for links array. links.bin by default
      });
});
