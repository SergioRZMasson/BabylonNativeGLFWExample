/// <reference path="../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

const fireColorsPalette = [{ "r": 7, "g": 7, "b": 7 }, { "r": 31, "g": 7, "b": 7 }, { "r": 47, "g": 15, "b": 7 }, { "r": 71, "g": 15, "b": 7 }, { "r": 87, "g": 23, "b": 7 }, { "r": 103, "g": 31, "b": 7 }, { "r": 119, "g": 31, "b": 7 }, { "r": 143, "g": 39, "b": 7 }, { "r": 159, "g": 47, "b": 7 }, { "r": 175, "g": 63, "b": 7 }, { "r": 191, "g": 71, "b": 7 }, { "r": 199, "g": 71, "b": 7 }, { "r": 223, "g": 79, "b": 7 }, { "r": 223, "g": 87, "b": 7 }, { "r": 223, "g": 87, "b": 7 }, { "r": 215, "g": 95, "b": 7 }, { "r": 215, "g": 95, "b": 7 }, { "r": 215, "g": 103, "b": 15 }, { "r": 207, "g": 111, "b": 15 }, { "r": 207, "g": 119, "b": 15 }, { "r": 207, "g": 127, "b": 15 }, { "r": 207, "g": 135, "b": 23 }, { "r": 199, "g": 135, "b": 23 }, { "r": 199, "g": 143, "b": 23 }, { "r": 199, "g": 151, "b": 31 }, { "r": 191, "g": 159, "b": 31 }, { "r": 191, "g": 159, "b": 31 }, { "r": 191, "g": 167, "b": 39 }, { "r": 191, "g": 167, "b": 39 }, { "r": 191, "g": 175, "b": 47 }, { "r": 183, "g": 175, "b": 47 }, { "r": 183, "g": 183, "b": 47 }, { "r": 183, "g": 183, "b": 55 }, { "r": 207, "g": 207, "b": 111 }, { "r": 223, "g": 223, "b": 159 }, { "r": 239, "g": 239, "b": 199 }, { "r": 255, "g": 255, "b": 255 }]

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

var numPerSide = 100, size = 200, ofst = size / (numPerSide - 2);

var col = 0, index = 0;

let instanceCount = numPerSide * numPerSide;

let matricesData = new Float32Array(16 * instanceCount);
let colorData = new Float32Array(4 * instanceCount);
let fireIntensityData = new Int32Array(instanceCount);

createFireDataStructure();

createCubes();

box.thinInstanceSetBuffer("matrix", matricesData, 16);
box.thinInstanceSetBuffer("color", colorData, 4);

box.material = new BABYLON.StandardMaterial("material");
box.material.disableLighting = true;
box.material.emissiveColor = BABYLON.Color3.White();


//JS update functions
scene.onBeforeCameraRenderObservable.add(() =>
{
    increaseFireSource();

    calculateFirePropagation();
    renderFire();
});

function createFireDataStructure() {
    for (let i = 0; i < instanceCount; i++) {
        fireIntensityData[i] = 0  
    }			    
}

function calculateFirePropagation() {
    for (let i = 0; i < instanceCount; i++) {
        updateFireIntensityPerPixel(i)
    }
}

function updateFireIntensityPerPixel(currentPixelIndex) {
    const belowPixelIndex = currentPixelIndex + numPerSide  // takes the reference value and adds a width

    // that way I can go to the pixel below
    // below pixel index overflows canvas
    if (belowPixelIndex >= numPerSide * numPerSide) {
        return
    }

    const decay = Math.floor(Math.random() * 4)  // fire intensity discount
    const belowPixelFireIntensity = fireIntensityData[belowPixelIndex]

    const newFireIntensity =
        belowPixelFireIntensity - decay >= 0 ? belowPixelFireIntensity - decay : 0  // don't show negative numbers

    // takes the new intensity value with discount and throws this value into the pixel that we are iterating
    let direction = Math.floor(Math.random() * 2) - 1  // get random direction 
    direction = (direction == 0) ? 1 : direction  // direction cant be zero 0
    const decayDirection = decay * direction // change fire direction
    fireIntensityData[currentPixelIndex - decayDirection] = newFireIntensity
    // fireIntensityData[currentPixelIndex - decay] = newFireIntensity  // orignal
}

function createCubes()
{
    let m = BABYLON.Matrix.Identity();

    for (var x = numPerSide - 1; x >= 0; x--) {
        m.m[13] = -size / 2 + ofst * x;
        for (var y = numPerSide-1; y >= 0; y--) {
            m.m[12] = -size / 2 + ofst * y;
            m.copyToArray(matricesData, index * 16);
            colorData[index * 4 + 0] = 0;
            colorData[index * 4 + 1] = 0;
            colorData[index * 4 + 2] = 0;
            colorData[index * 4 + 3] = 1.0;
            index++;
        }
    }
}

function renderFire()
{
    for (let instanceID = 0; instanceID < instanceCount; instanceID++) {

        let fireIntensity = fireIntensityData[instanceID];
        let fireColor = fireColorsPalette[fireIntensity];

        colorData[instanceID * 4 + 0] = fireColor.r / 255;
        colorData[instanceID * 4 + 1] = fireColor.g / 255;
        colorData[instanceID * 4 + 2] = fireColor.b / 255;
    }

    box.thinInstanceSetBuffer("color", colorData, 4);
    box.thinInstanceBufferUpdated("color");
}

function increaseFireSource() {
    let fireWidth = numPerSide;
    let fireHeight = numPerSide;

    for (let column = 0; column <= fireWidth; column++) {
        const overflowPixelIndex = fireWidth * fireHeight  // all the pixels of the fire
        const pixelIndex = (overflowPixelIndex - fireWidth) + column  //find last pixel of the colunm
        const currentFireIntensity = fireIntensityData[pixelIndex]

        // increases the pixel according to its intensity
        if (currentFireIntensity < 36) {
            const increase = Math.floor(Math.random() * 7)
            const newFireIntensity =
                currentFireIntensity + increase >= 36 ? 36 : currentFireIntensity + increase

            fireIntensityData[pixelIndex] = newFireIntensity
        }
    }
}

function decreaseFireSource() {
    for (let column = 0; column <= fireWidth; column++) {
        const overflowPixelIndex = fireWidth * fireHeight  // all the pixels of the fire
        const pixelIndex = (overflowPixelIndex - fireWidth) + column  //find last pixel of the colunm
        const currentFireIntensity = fireIntensityData[pixelIndex]

        // increases the pixel according to its intensity
        if (currentFireIntensity > 0) {
            const decay = Math.floor(Math.random() * 7)  // fire intensity discount
            const newFireIntensity =
                currentFireIntensity - decay >= 0 ? currentFireIntensity - decay : 0

            // takes the new intensity value with discount and throws this value into the pixel that we are iterating
            fireIntensityData[pixelIndex] = newFireIntensity
        }
    }
}