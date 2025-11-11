class FileSystem
{
    static async promptSaveFileBuffer(buffer, filename, mimeType) {
        if (!window.showSaveFilePicker) {
            const blob = new Blob([buffer], { type: mimeType });
            const url  = URL.createObjectURL(blob);
            const a    = document.createElement('a');
            a.href     = url;
            a.download = filename;

            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);

            URL.revokeObjectURL(url);
            return;
        }

        const opts = {
            suggestedName: filename,
            types: [
                {
                    description: mimeType,
                    accept: { [mimeType]: ['.' + filename.split('.').pop()] }
                }
            ]
        };

        const handle   = await window.showSaveFilePicker(opts);
        const writable = await handle.createWritable();
        await writable.write(buffer);
        await writable.close();
    }
    

    static promptLoadFileBuffer(extensionHint = '*') {
        return new Promise((resolve, reject) => {
            const input  = document.createElement('input');
            input.type   = 'file';
            input.accept = extensionHint;
            input.click();

            input.addEventListener('change', _ => {
                const reader = new FileReader();
                reader.onload  = _ => resolve(new Uint8Array(reader.result));
                reader.onerror = _ => reject();

                reader.readAsArrayBuffer(input.files[0]);
            });

            input.addEventListener('cancel', _ => reject());
        })
    }
}

export { FileSystem };