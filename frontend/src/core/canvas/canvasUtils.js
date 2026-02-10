export class CanvasUtils {
    static snapToPureBitonal(ctx, x, y, width, height) {
        x = Math.max(0, x);
        y = Math.max(0, y);

        const data = ctx.getImageData(x, y, width, height, {
            colorSpace: 'srgb',
            pixelFormat: 'rgba-unorm8'});

        for (let i = 0; i < data.data.length; i += 4) {
            if (data.data[i + 0] < 240 ||
                data.data[i + 1] < 240 ||
                data.data[i + 2] < 240) {
                data.data[i + 0] = 0;
                data.data[i + 1] = 0;
                data.data[i + 2] = 0;
            } else {
                data.data[i + 0] = 255;
                data.data[i + 1] = 255;
                data.data[i + 2] = 255;
            }
        }

        ctx.putImageData(data, x, y);
    }
};