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
            const input = document.createElement('input');
            input.type   = 'file';
            input.accept = extensionHint;
            input.click();

            /*
             *      Modern browsers should clean up event listeners when element's removed
             *      from DOM.
             */
            const cleanup = fn => { input.remove(); fn(); };

            input.addEventListener('change', _ => {
                const reader = new FileReader();
                reader.onload  = _ => cleanup(() => resolve(new Uint8Array(reader.result)));
                reader.onerror = _ => cleanup(() => reject());
                reader.readAsArrayBuffer(input.files[0]);
            });

            input.addEventListener('cancel', _ => cleanup(() => reject));
        });
    }
}

export { FileSystem };