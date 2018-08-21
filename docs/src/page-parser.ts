import * as fs from 'fs';

const baseFolder = 'pages';
const parsedFolder = '..';

const templateFile = fs.readFileSync('template.html').toString();
const files = fs.readdirSync(baseFolder);

for (const file of files) {
    fs.readFile(`${baseFolder}/${file}`, (err, data) => {
        if (err) {
            throw err;
        }

        const fileContent = data.toString();
        const parsedContent = templateFile.replace('{{content}}', fileContent);

        fs.writeFile(`${parsedFolder}/${file}`, parsedContent, writeErr => {
            if (writeErr) {
                throw writeErr;
            }
        });
    });
}
