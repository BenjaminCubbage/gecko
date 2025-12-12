import fs from "fs";
import path from "path";

console.log(import.meta.dirname);

const root    = path.resolve(import.meta.dirname, "../..");

const srcDir  = path.join(root, "/build/artifacts/wasm");
const destDir = path.join(root, "/frontend/public/wasm");

const src1  = path.join(srcDir,  "/compression.js");
const src2  = path.join(srcDir,  "/compression.wasm");
const dest1 = path.join(destDir, "/compression.js");
const dest2 = path.join(destDir, "/compression.wasm");

if (!fs.existsSync(destDir))
    fs.mkdirSync(destDir);
fs.copyFileSync(src1, dest1);
fs.copyFileSync(src2, dest2);

console.log(`Copied ${src1} → ${dest1}`);
console.log(`Copied ${src2} → ${dest2}`);