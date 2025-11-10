import fs from "fs";
import path from "path";

const root = path.resolve(import.meta.dirname, "../");
const src1 = path.join(root, "build/shared/compression/wasm/compression.js");
const src2 = path.join(root, "build/shared/compression/wasm/compression.wasm");

const dest1 = path.join(root, "frontend/public/wasm/compression.js");
const dest2 = path.join(root, "frontend/public/wasm/compression.wasm");

fs.copyFileSync(src1, dest1);
fs.copyFileSync(src2, dest2);
console.log(`Copied ${src1} → ${dest1}`);
console.log(`Copied ${src2} → ${dest2}`);