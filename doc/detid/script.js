let detectors = [];

fetch('detector_ids.json')
    .then(response => response.json())
    .then(data => {
        detectors = data;
        populateTable(detectors);
    });

function populateTable(data) {
    const tableBody = document.getElementById('detectorTable').getElementsByTagName('tbody')[0];
    tableBody.innerHTML = '';
    data.forEach(det => {
        let row = tableBody.insertRow();
        let nameCell = row.insertCell();
        nameCell.textContent = det.name;
        let idCell = row.insertCell();
        idCell.textContent = det.id;
        let fileCell = row.insertCell();
        fileCell.textContent = det.file;
        let lineCell = row.insertCell();
        lineCell.textContent = det.line;
    });
}

function searchTable() {
    const nameFilter = document.getElementById('nameSearch').value.toLowerCase();
    const idFilter = document.getElementById('idSearch').value.toLowerCase();

    const filteredData = detectors.filter(det => {
        const nameMatch = det.name.toLowerCase().includes(nameFilter);
        const idMatch = det.id.toString().toLowerCase().includes(idFilter);
        return nameMatch && idMatch;
    });

    populateTable(filteredData);
}
