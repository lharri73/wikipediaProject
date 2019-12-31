/**
 * This is a core piece of our application. It fetches and provides graph data.
 */
var eventify = require('ngraph.events');
var createGraph = require('ngraph.graph');
var subgraph = require('./model/subgraph');
var config = require('./config.js');
var manifestEndpoint = config.dataUrl + 'manifest.json?nocache=' + (+new Date());

var hardLocation = new Promise((resolve, reject) =>{
  resolve('//lharri73.github.io/wikipediaProject/data/positions.bin');
});

module.exports = function($http) {
  var graph = createGraph({
    uniqueLinkId: false // don't need unique link id, since we are not a multigraph
  });
  var filteredGraph = graph;
  var labels;
  var dataEndpoint;
  var attributes = {
    links: false,
    labels: false,
    sizes: false,
    levels: false,
  }
  hardLocation
    // .then(setDataEndpoint)
    .then(loadPositions)
    .then(convertToPositions)
    .then(addNodesToGraph)
    .then(downloadAttributesAsync)
    .catch (reportError);

  var model = {
    getGraph: function getGraph() {
      return filteredGraph;
    },

    filterSubgraph: filterSubgraph,

    getNodeByName: getNodeByName,

    getPackagePosition: function(packageName) {
      var node = getNodeByName(packageName);
      return node && node.data.position;
    },

    filter: function(pattern) {
      if (!pattern) {
        filteredGraph = graph;
        return;
      }

      filteredGraph = createGraph({ uniqueLinkId: false });

      var matcher = require('./search/matcher')(pattern);
      var idx = 0;
      graph.forEachNode(function(node) {
        if (matcher.isMatch(node.data)) {
          filteredGraph.addNode(idx, node.data);
          idx += 1;
        }
      });
    }
  };

  eventify(model);

  // we want to notify on next event cycle, that nodes are being loaded
  // Since this class is on the core ones, it needs to let other code to be
  // bootstrapped, before it can fire events.
  setTimeout(function() {
    model.fire('loadingNodes');
  }, 0);

  return model;

  function reportError(err) {
    model.fire('downloadFailed', err);
  }

  function filterSubgraph(packageName, type) {
    var id = packageNameToId(packageName);
    if (id < 0) return;

    var needAll = type && (type[0] === 'a') && (type[1] === 'l') && (type[2] === 'l');
    if (needAll) {
      type = type.substr(3); // remove `all` prefix
    }

    if (type === 'dependents') {
      filteredGraph = needAll ? subgraph.inAll(graph, id) : subgraph.in(graph, id);
    } else {
      filteredGraph = needAll ? subgraph.outAll(graph, id) : subgraph.out(graph, id);
    }

    return filteredGraph;
  }

  function getNodeByName(packageName) {
    var id = packageNameToId(packageName);
    if (id < 0) return;

    return graph.getNode(id);
  }

  function packageNameToId(packageName) {
    if (!labels) return -1; // without labels we cannot do anything
    // doh O(n). Should I care?
    return labels.indexOf(packageName);
  }

  function downloadAttributesAsync() {
    model.fire('loadingConnections');

    // Note: we are not returning a promise here. This is supposed to be
    // fire and forget call.
    $http.get('//lharri73.github.io/wikipediaProject/data/links.bin', {
      responseType: "arraybuffer"
    })
      .then(addLinksToGraph)
      .then(downloadLabelsAsync)
      .then(downloadSizesAsync)
      .then(downloadClickLevelsAsync)
      .then(notifyCoreReady);
  }

  function addLinksToGraph(res) {
    var arr = new Int32Array(res.data);
    var lastFromId;
    for (var i = 0; i < arr.length; i++) {
      var id = arr[i];
      if (id < 0) {
        lastFromId = -id - 1;
      } else {
        graph.addLink(lastFromId, id - 1);
      }
    }
    attributes.links = true;
    return graph;
  }

  function downloadLabelsAsync() {
    $http.get('//lharri73.github.io/wikipediaProject/data/labels.json')
      .then(addLabelsToGraph);
  }

  function downloadSizesAsync() {
    $http.get('//lharri73.github.io/wikipediaProject/data/sizes.json')
      .then(addSizesToGraph);
  }

  function addSizesToGraph(res){
    sizes = res.data;
    sizes.forEach(function(size, idx){
      addToGraph(idx, 'size', size);
    });
    attributes.sizes = true;
    notify_if_needed();
  }

  function downloadClickLevelsAsync() {
    $http.get('//lharri73.github.io/wikipediaProject/data/clickLevels.json')
      .then(addClickLevelsToGraph);
  }

  function addClickLevelsToGraph(res) {
    levels = res.data;
    levels.forEach(function(level, idx){
      addToGraph(idx, 'clickLevel', level);
    })
    attributes.levels = true;
    notify_if_needed();
  }

  function addLabelsToGraph(response) {
    labels = response.data;
    labels.forEach(function(label, idx) {
      addToGraph(idx, 'label', label);
    });
    
    attributes.labels = true;
    notify_if_needed();
  }

  function addNodesToGraph(positions) {
    positions.forEach(function(pos, idx) {
      addToGraph(idx, 'position', pos);
    });

    model.fire('nodesReady', model);
  }

  function notifyCoreReady() {
    model.fire('coreReady');
  }

  function notify_if_needed(){
    if(attributes.links && attributes.sizes && attributes.levels && attributes.labels){
      model.fire('allReady', model);
    }
  }


  function setDataEndpoint(manifestResponse) {
    var manifest = manifestResponse.data;
    dataEndpoint = config.dataUrl + manifest.last + '/'
  }

  function loadPositions(locationResponse) {
    return $http.get(locationResponse, {
      responseType: 'arraybuffer'
    })
  }

  function convertToPositions(response) {
    var data = new Int32Array(response.data);
    var positions = [];

    for (var i = 0; i < data.length; i += 3) {
      var pos = {
        x: data[i],
        y: data[i + 1],
        z: data[i + 2]
      };
      positions.push(pos);
    }

    return positions;
  }

  function addToGraph(nodeId, dataName, dataValue) {
    var node = graph.getNode(nodeId);
    if (!node) {
      var data = {};
      data[dataName] = dataValue;
      graph.addNode(nodeId, data);
    } else {
      node.data[dataName] = dataValue;
    }
  }
};
