declare const $: any;

window.addEventListener('load', () => {
    const sidebar = document.getElementById('sidebar') as HTMLDivElement;
    const content = document.getElementById('content') as HTMLDivElement;
    const links = sidebar.getElementsByTagName('a');

    for (let i = 0; i < links.length; i++) {
        const link = links[i];
        link.addEventListener('click', e => {
            const relativeUrl = link.href.substr(link.href.indexOf('docs/'));
            $(content).load(relativeUrl);
            e.preventDefault();
        });
    }
});
