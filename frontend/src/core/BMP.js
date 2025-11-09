export default class BMP {
  constructor(arrayBuffer) {
    this.buffer = arrayBuffer;
  }

  save() {
    const blob = new Blob([this.buffer], { type: 'image/bmp' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'image.bmp';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }
}
