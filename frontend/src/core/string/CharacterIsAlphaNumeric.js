const cca = 'a'.charCodeAt(0);
const ccz = 'z'.charCodeAt(0);
const ccA = 'A'.charCodeAt(0);
const ccZ = 'Z'.charCodeAt(0);
const cc0 = '0'.charCodeAt(0);
const cc9 = '9'.charCodeAt(0);

export const characterIsAlphaNumeric = (c) => {
    if (!c || !c.charCodeAt)
        return false;

    const charCode = c.charCodeAt(0);

    return (charCode >= cca && charCode <= ccz) ||
           (charCode >= ccA && charCode <= ccZ) ||
           (charCode >= cc0 && charCode <= cc9);
};