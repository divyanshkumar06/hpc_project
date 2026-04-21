// Wait for DOM
document.addEventListener('DOMContentLoaded', () => {
    if (typeof dashboardData === 'undefined') {
        console.error("Dashboard data not found! Ensure python benchmark script ran successfully.");
        document.querySelector('.table-container').innerHTML = "<p style='color: #ef4444'>Error: Run the python bridge script to generate data.js first.</p>";
        return;
    }

    const { metrics, cpuCores } = dashboardData;

    // --- POPULATE TABLE ---
    const tbody = document.querySelector('#metricsTable tbody');
    let maxSpeedup = 0;

    metrics.forEach(row => {
        const tr = document.createElement('tr');
        
        // Auto-Diagnose Bottlenecks
        let badgeHtml = '';
        if (row.filter.includes('AVX2') || row.parallelTime < 0.005) {
            badgeHtml = '<span class="badge badge-instant">⚡ AVX2 Instantaneous</span>';
        } else if (row.efficiency >= 0.85 && row.threads > 1) {
            badgeHtml = '<span class="badge badge-compute">🚀 Compute-Bound (Perfect Scaling)</span>';
        } else if (row.efficiency < 0.85 && row.threads > 2) {
            badgeHtml = '<span class="badge badge-memory">🔀 Memory Bandwidth Limit</span>';
        } else {
            badgeHtml = '<span style="color: #64748b; font-size: 0.8rem;">Baseline</span>';
        }

        tr.innerHTML = `
            <td>${row.filter}</td>
            <td>${row.threads}</td>
            <td>${row.serialTime.toFixed(4)}</td>
            <td>${row.parallelTime.toFixed(4)}</td>
            <td>${row.speedup.toFixed(2)}x</td>
            <td style="color: ${getEfficiencyColor(row.efficiency)}">${(row.efficiency * 100).toFixed(1)}%</td>
            <td>${badgeHtml}</td>
        `;
        tbody.appendChild(tr);

        if (row.speedup > maxSpeedup && row.speedup !== Infinity) {
            maxSpeedup = row.speedup;
        }
    });

    document.getElementById('peak-speedup').textContent = `${maxSpeedup.toFixed(2)}x`;

    // --- INTERACTIVE SPLIT SLIDER ---
    const filterButtons = document.querySelectorAll('.filter-selector button');
    const processedImg = document.getElementById('img-processed');
    
    // Slider Core Elements
    const sliderContainer = document.getElementById('slider-container');
    const sliderOverlay = document.getElementById('slider-overlay');
    const sliderHandle = document.getElementById('slider-handle');
    let isSliding = false;

    // Slide function to update widths
    const slide = (e) => {
        if (!isSliding) return;
        
        let clientX = e.clientX || (e.touches && e.touches[0].clientX);
        let rect = sliderContainer.getBoundingClientRect();
        let x = clientX - rect.left;
        let percentage = (x / rect.width) * 100;
        
        // Clamp bounds
        if (percentage < 0) percentage = 0;
        if (percentage > 100) percentage = 100;
        
        sliderOverlay.style.width = `${percentage}%`;
        sliderHandle.style.left = `${percentage}%`;
    };

    // Event Listeners for Slider
    sliderHandle.addEventListener('mousedown', () => isSliding = true);
    sliderHandle.addEventListener('touchstart', () => isSliding = true);
    
    window.addEventListener('mouseup', () => isSliding = false);
    window.addEventListener('touchend', () => isSliding = false);
    
    window.addEventListener('mousemove', slide);
    window.addEventListener('touchmove', slide);

    // Filter Buttons logic update
    filterButtons.forEach(btn => {
        btn.addEventListener('click', (e) => {
            filterButtons.forEach(b => b.classList.remove('active'));
            e.target.classList.add('active');

            const filterName = e.target.getAttribute('data-filter');
            const isTiled = e.target.getAttribute('data-tiled') === 'true';
            
            // For tiled, there's no output_tiled.jpg, we just use gaussian 
            const imgSrc = isTiled ? '../output_gaussian.jpg' : `../output_${filterName}.jpg`;
            processedImg.src = imgSrc;
            
            // Auto reset slider to center on change
            sliderOverlay.style.width = '50%';
            sliderHandle.style.left = '50%';
        });
    });

    // --- CHARTS CONFIGURATION ---
    Chart.defaults.color = '#94a3b8';
    Chart.defaults.font.family = "'Inter', sans-serif";

    // Extract data for charts (Scalar as representative example)
    const chartData = metrics.filter(m => m.filter === 'Gaussian Blur (Scalar)');
    const threads = chartData.map(m => m.threads);
    const speedups = chartData.map(m => m.speedup);
    const execTimes = chartData.map(m => m.parallelTime);
    const idealSpeedups = threads.map(t => t); // Linear scaling

    // 1. Time Chart
    const ctxTime = document.getElementById('timeChart').getContext('2d');
    new Chart(ctxTime, {
        type: 'line',
        data: {
            labels: threads,
            datasets: [{
                label: 'Parallel Execution Time (s)',
                data: execTimes,
                borderColor: '#38bdf8',
                backgroundColor: 'rgba(56, 189, 248, 0.1)',
                borderWidth: 3,
                tension: 0.4,
                fill: true,
                pointBackgroundColor: '#0f172a',
                pointBorderColor: '#38bdf8',
                pointRadius: 6
            }]
        },
        options: {
            responsive: true,
            scales: {
                y: {
                    beginAtZero: true,
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    title: { display: true, text: 'Seconds' }
                },
                x: {
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    title: { display: true, text: 'Number of Threads' }
                }
            }
        }
    });

    // 2. Speedup Chart
    const ctxSpeedup = document.getElementById('speedupChart').getContext('2d');
    new Chart(ctxSpeedup, {
        type: 'line',
        data: {
            labels: threads,
            datasets: [
                {
                    label: 'Actual Speedup',
                    data: speedups,
                    borderColor: '#818cf8',
                    borderWidth: 3,
                    tension: 0.4,
                    pointBackgroundColor: '#0f172a',
                    pointBorderColor: '#818cf8',
                    pointRadius: 6
                },
                {
                    label: 'Ideal Speedup (Linear)',
                    data: idealSpeedups,
                    borderColor: 'rgba(255,255,255,0.3)',
                    borderWidth: 2,
                    borderDash: [5, 5],
                    pointRadius: 0
                }
            ]
        },
        options: {
            responsive: true,
            scales: {
                y: {
                    beginAtZero: true,
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    title: { display: true, text: 'Speedup Factor' }
                },
                x: {
                    grid: { color: 'rgba(255,255,255,0.05)' },
                    title: { display: true, text: 'Number of Threads' }
                }
            }
        }
    });
});

function getEfficiencyColor(eff) {
    if (eff >= 0.8) return '#4ade80'; // Green
    if (eff >= 0.5) return '#facc15'; // Yellow
    return '#ef4444'; // Red
}
