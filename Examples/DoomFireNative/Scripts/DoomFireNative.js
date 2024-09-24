/// <reference path="../../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

const numPerSide = 100;
const size = 200;
const ofst = size / (numPerSide - 2);
const instanceCount = numPerSide * numPerSide * numPerSide;
const matricesData = new Float32Array(16 * instanceCount);
const colorData = new Float32Array(4 * instanceCount);

var engine = new BABYLON.NativeEngine();
var scene = new BABYLON.Scene(engine);

engine.runRenderLoop(function () {
    scene.render();
});

//-------------------- YOUR CODE GOES HERE ------------------------------
var camera = new BABYLON.ArcRotateCamera("Camera", -Math.PI / 2, Math.PI / 2, 250, BABYLON.Vector3.Zero(), scene);
camera.attachControl(true);

// This creates a light, aiming 0,1,0 - to the sky (non-mesh)
var light = new BABYLON.HemisphericLight("light", new BABYLON.Vector3(0, 1, 0), scene);

// Default intensity is 1. Let's dim the light a small amount
light.intensity = 0.7;

var box = BABYLON.MeshBuilder.CreateBox("root", { size: 1.5 });

// Native functions
createCubes();
createNativeBuffers(instanceCount);

box.thinInstanceSetBuffer("matrix", matricesData, 16);
box.thinInstanceSetBuffer("color", colorData, 4);

box.material = new BABYLON.StandardMaterial("material");
box.material.disableLighting = true;
box.material.emissiveColor = BABYLON.Color3.White();

//JS update functions
scene.onBeforeCameraRenderObservable.add(() => {
    nativeUpdate(colorData, numPerSide);
    box.thinInstanceSetBuffer("color", colorData, 4);
    box.thinInstanceBufferUpdated("color");
});

function createCubes() {
    var col = 0, index = 0;
    let m = BABYLON.Matrix.Identity();
    for (var x = numPerSide - 1; x >= 0; x--) {
        m.m[13] = -size / 2 + ofst * x;
        for (var y = numPerSide - 1; y >= 0; y--) {
            m.m[12] = -size / 2 + ofst * y;
            for (var z = 0; z < numPerSide; z++) {
                m.m[14] = -size / 2 + ofst * z;
                m.copyToArray(matricesData, index * 16);
                colorData[index * 4 + 0] = 0;
                colorData[index * 4 + 1] = 0;
                colorData[index * 4 + 2] = 0;
                colorData[index * 4 + 3] = 1.0;
                index++;
            }
        }
    }
}
