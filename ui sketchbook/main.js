(function () {

    // ---------- state ----------
    const settings = {
        screenW: 128, screenH: 64,
        zoom: 6, showGrid: false,
        funcName: 'drawMenuScreen',
        gVar: 'graphics',
        onConst: 'WHITE', offConst: 'BLACK',
        clearDisplay: true, callDisplay: true,
    };

    let elements = [];       // {id,type,label,...}
    let selectedId = null;
    let nextId = 1;
    const typeCounters = { rect: 0, rrect: 0, line: 0, text: 0, bitmap: 0 , circle:0};

    const typeName = { circle:'Circle',rect: 'Rect', rrect: 'Round Rect', line: 'Line', text: 'Text', bitmap: 'Bitmap' };

    function makeElement(type) {
        typeCounters[type]++;
        const n = typeCounters[type];
        const base = { id: nextId++, type, label: `${typeName[type]} ${n}`, on: true };
        const offset = ((n - 1) % 6) * 6;
        switch (type) {
            case 'circle':
                return Object.assign(base, { x: 10 + offset, y: 8 + offset, r:18, filled: false });
            case 'rect':
                return Object.assign(base, { x: 10 + offset, y: 8 + offset, w: 40, h: 18, filled: false });
            case 'rrect':
                return Object.assign(base, { x: 10 + offset, y: 8 + offset, w: 40, h: 18, r: 4, filled: false });
            case 'line':
                return Object.assign(base, { x1: 8 + offset, y1: 8 + offset, x2: 48 + offset, y2: 24 + offset });
            case 'text':
                return Object.assign(base, { x: 10 + offset, y: 8 + offset, text: 'Text', size: 1 });
            case 'bitmap':
                return Object.assign(base, { x: 10 + offset, y: 8 + offset, w: 16, h: 16, name: 'icon_bitmap' });
        }
    }

    function selectedEl() { return elements.find(e => e.id === selectedId) || null; }

    // ---------- geometry helpers ----------
    function textDims(el) {
        return { w: Math.max(1, el.text.length) * 6 * el.size, h: 8 * el.size };
    }
    function bbox(el) {
        if (el.type === 'line') {
            return {
                x: Math.min(el.x1, el.x2), y: Math.min(el.y1, el.y2),
                w: Math.max(1, Math.abs(el.x2 - el.x1)), h: Math.max(1, Math.abs(el.y2 - el.y1))
            };
        }
        if(el.type === 'circle'){
            return {
                x: el.x - el.r,
                y: el.y - el.r,
                w: 2*el.r
            };
        }
        if (el.type === 'text') {
            const d = textDims(el);
            return { x: el.x, y: el.y, w: d.w, h: d.h };
        }
        return { x: el.x, y: el.y, w: el.w, h: el.h };
    }
    function clamp(v, lo, hi) { return Math.max(lo, Math.min(hi, v)); }

    // ---------- canvas setup ----------
    const canvas = document.getElementById('main_canvas');
    const ctx = canvas.getContext('2d');

    function resizeCanvas() {
        document.documentElement.style.setProperty('--canvas-width', `${settings.screenW * settings.zoom}px`);
        document.documentElement.style.setProperty('--canvas-height', `${settings.screenH * settings.zoom}px`);
        canvas.width = settings.screenW;
        canvas.height = settings.screenH;
    }

    function render() {
        document.documentElement.style.setProperty('--pixel-grid-visibility', settings.showGrid?'visible':'hidden');

        resizeCanvas();

        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        elements.forEach(el => drawElement(el));

        updateSelection();
    }

    function drawElement(el) {
        const on = el.on;
        const fg = on ? '#ffffff' : '#000000';
        ctx.fillStyle = fg; ctx.strokeStyle = fg; ctx.lineWidth = Math.max(1, 0.16);

        if (el.type === 'rect') {
            if (el.filled) fillRect(el.x,el.y,el.w,el.h);
            else ctx.strokeRect(el.x + ctx.lineWidth / 2, el.y + ctx.lineWidth / 2, el.w - ctx.lineWidth, el.h - ctx.lineWidth);
        } else if (el.type === 'rrect') {
            // if(el.filled)
            //     fillRoundRect(el.x,el.y,el.w,el.h,el.r);
            // else
                drawRoundRect(el.x,el.y,el.w,el.h,el.r,1);
        } else if (el.type === 'line') {
            drawLine(el.x1,el.y1,el.x2,el.y2,on?1:0);
        }
        else if(el.type === 'circle'){

        }else if (el.type === 'text') {
            ctx.fillStyle = fg;
            ctx.font = `${8 * el.size}px monospace`;
            ctx.textBaseline = 'top';
            ctx.fillText(el.text, el.x, el.y);
        } else if (el.type === 'bitmap') {
            ctx.strokeStyle = on ? '#f2fffa' : '#3a3f46';
            ctx.setLineDash([0.4, 0.3]);
            ctx.strokeRect(el.x, el.y, el.w, el.h);
            ctx.setLineDash([]);
            ctx.beginPath();
            ctx.moveTo(el.x, el.y); ctx.lineTo((el.x + el.w), (el.y + el.h));
            ctx.moveTo((el.x + el.w), el.y); ctx.lineTo(el.x, (el.y + el.h));
            ctx.stroke();
            ctx.fillStyle = '#7c8896';
            ctx.font = `${Math.max(8, 1.3)}px monospace`;
            ctx.textBaseline = 'bottom';
            ctx.fillText(el.name, el.x + 2, el.y - 2);
        }
    }

    // adapted from Adafruit_GFX.cpp
    function drawRoundRect( x,  y,  w,  h, r, color) {
        let max_radius = Math.trunc(((w < h) ? w : h) / 2); // 1/2 minor axis
        if (r > max_radius)
        r = max_radius;
        // smarter version
        drawLine(x+r,y,x+w-r,y,color);
        drawLine(x+r,y+h-1,x+w-r,y+h-1,color);
        drawLine(x,y+r,x,y+h-r,color);
        drawLine(x+w-1,y+r,x+w-1,y+h-r,color);
        // draw four corners
        drawCircleHelper(x + r, y + r, r, 1, color);
        drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
        drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
        drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
    }

    function drawCircleHelper(x0, y0, r, cornername, color) {
        let f = 1 - r;
        let ddF_x = 1;
        let ddF_y = -2 * r;
        let x = 0;
        let y = r;

        while (x < y) {
            if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
            }
            x++;
            ddF_x += 2;
            f += ddF_x;
            if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
            }
            if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
            }
            if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
            }
            if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
            }
        }
    }

    function fillRoundRect( x,  y,  w,  h, r, color) {
        let max_radius = Math.trunc(((w < h) ? w : h) / 2); // 1/2 minor axis
        if (r > max_radius)
        r = max_radius;
        // smarter version
        // drawLine(x+r,y,x+w-r,y,color);
        // drawLine(x+r,y+h-1,x+w-r,y+h-1,color);
        // drawLine(x,y+r,x,y+h-r,color);
        // drawLine(x+w-1,y+r,x+w-1,y+h-r,color);
        fillRect(x + r, y, w - 2 * r, h,color);
        // draw four corners
        fillCircleHelper(x + r, y + r, r, 1, color);
        fillCircleHelper(x + w - r - 1, y + r, r, 2, color);
        fillCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
        fillCircleHelper(x + r, y + h - r - 1, r, 8, color);
    }

    function fillCircleHelper(x0, y0, r, corners, delta, color) {

        let f = 1 - r;
        let ddF_x = 1;
        let ddF_y = -2 * r;
        let x = 0;
        let y = r;
        let px = x;
        let py = y;

        delta++; // Avoid some +1's in the loop

        while (x < y) {
            if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
            }
            x++;
            ddF_x += 2;
            f += ddF_x;
            // These checks avoid double-drawing certain lines, important
            // for the SSD1306 library which has an INVERT drawing mode.
            if (x < (y + 1)) {
                if (corners & 1)
                    drawLine(x0+x,y0-y,x0+x,y0+y+delta,color);
                    // writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
                if (corners & 2)
                    drawLine(x0-x,y0-y,x0-x,y0+y+delta,color);
                    // writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
                }
                if (y != py) {
                if (corners & 1)
                    drawLine(x0+py,y0-px,x0+py,y0+px+delta,color);
                    // writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
                if (corners & 2)
                    drawLine(x0-py,y0-px,x0-py,y0+px+delta,color);
                    // writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
                py = y;
            }
            px = x;
        }
    }

    function drawRect(x,y,w,h,color){
        // const x2 = x+w;
        // const y2 = y+h;
        // drawLine(x,y,x2,y,color);//top
        // drawLine(x2,y,x2,y2,color);//right
        // drawLine(x,y2,x2,y2,color);//bottom
        // drawLine(x,y,x,y2,color);//left
    }

    function fillRect(x,y,w,h,color){
        ctx.fillRect(x,y,w,h);
    }

    function drawPixel(x, y, color) {
      ctx.fillStyle = (color == 'WHITE' || color)? '#ffffff':'#000000';
      ctx.fillRect(x, y, 1, 1);
    }

    function drawLine(x0, y0, x1, y1, val) {
        const steep = Math.abs(y1 - y0) > Math.abs(x1 - x0);
        if (steep) {
            [x0, y0] = [y0, x0];
            [x1, y1] = [y1, x1];
        }

        if (x0 > x1) {
            [x0, x1] = [x1, x0];
            [y0, y1] = [y1, y0];
        }

        let dx, dy;
        dx = x1 - x0;
        dy = Math.abs(y1 - y0);

        let err = Math.trunc(dx / 2);
        let ystep;

        if (y0 < y1) {
            ystep = 1;
        } else {
            ystep = -1;
        }
        let y = y0;
        for (let x = x0; x <= x1; x++) {
            if (steep) {
                drawPixel(y, x, val);
            } else {
                drawPixel(x, y, val);
            }
            err -= dy;
            if (err < 0) {
                y += ystep;
                err += dx;
            }
        }
    }

    function roundRectPath(x, y, w, h, r) {
        r = Math.min(r, w / 2, h / 2);
        ctx.beginPath();
        ctx.moveTo(x + r, y);
        ctx.arcTo(x + w, y, x + w, y + h, r);
        ctx.arcTo(x + w, y + h, x, y + h, r);
        ctx.arcTo(x, y + h, x, y, r);
        ctx.arcTo(x, y, x + w, y, r);
        ctx.closePath();
    }

    const HANDLE = 5;
    function updateSelection() {
        const el = selectedEl();
        if(el){
            console.log(el);
            if(el.type == 'line'){
                el.w = Math.abs(el.x2 - el.x1);
                el.h = Math.abs(el.y2 - el.y1);
                el.x = Math.min(el.x1,el.x2);
                el.y = Math.min(el.y1,el.y2);
                el.bottomR = {
                    x : el.x2,
                    y : el.y2
                }
                el.topL = {
                    x : el.x1,
                    y : el.y1
                }
                
            }
            else{
                el.bottomR = {
                    x : el.x+el.w,
                    y : el.y+el.h
                }
                el.topL = {
                    x : el.x,
                    y : el.y
                }
            }

            document.documentElement.style.setProperty('--selection-box-visibility', `visible`);
            document.documentElement.style.setProperty('--selection-box-width', `${settings.zoom * el.w}px`);
            document.documentElement.style.setProperty('--selection-box-height', `${settings.zoom * el.h}px`);
            document.documentElement.style.setProperty('--selection-handle-top-left-x', `${settings.zoom * el.topL.x}px`);
            document.documentElement.style.setProperty('--selection-handle-top-left-y', `${settings.zoom * el.topL.y}px`);
            document.documentElement.style.setProperty('--selection-handle-bottom-right-x', `${settings.zoom * el.bottomR.x}px`);
            document.documentElement.style.setProperty('--selection-handle-bottom-right-y', `${settings.zoom * el.bottomR.y}px`);
            document.getElementById('selected_element_label').innerText = el.label;
        }
        else
            document.documentElement.style.setProperty('--selection-box-visibility', `hidden`);
    }

    // ---------- hit testing ----------
    function hitResizeHandle(el, lx, ly) {
        if (el.type === 'text' || el.type === 'line') return false;
        const b = bbox(el);
        const hx = b.x + b.w, hy = b.y + b.h;
        const tol = 6;
        return Math.abs(lx - hx) <= tol && Math.abs(ly - hy) <= tol;
    }
    function hitLineEndpoint(el, lx, ly) {
        const tol = 6;
        if (Math.abs(lx - el.x1) <= tol && Math.abs(ly - el.y1) <= tol) return 'p1';
        if (Math.abs(lx - el.x2) <= tol && Math.abs(ly - el.y2) <= tol) return 'p2';
        return null;
    }
    function distToSegment(px, py, ax, ay, bx, by) {
        const dx = bx - ax, dy = by - ay;
        const len2 = dx * dx + dy * dy;
        let t = len2 === 0 ? 0 : ((px - ax) * dx + (py - ay) * dy) / len2;
        t = clamp(t, 0, 1);
        const cx = ax + t * dx, cy = ay + t * dy;
        return Math.hypot(px - cx, py - cy);
    }
    function hitElement(lx, ly) {
        for (let i = elements.length - 1; i >= 0; i--) {
            const el = elements[i];
            if (el.type === 'line') {
                if (distToSegment(lx, ly, el.x1, el.y1, el.x2, el.y2) <= 3) return el;
            } else {
                const b = bbox(el);
                if (lx >= b.x && lx <= b.x + b.w && ly >= b.y && ly <= b.y + b.h) return el;
            }
        }
        return null;
    }

    // ---------- mouse interaction ----------
    let drag = null; // {mode, el, startLX, startLY, snap:{...}}

    function localCoords(e) {
        const rect = canvas.getBoundingClientRect();
        return {
            lx: (e.clientX - rect.left)/settings.zoom,
            ly: (e.clientY - rect.top)/settings.zoom
        };
    }

    canvas.addEventListener('mousedown', e => {
        const { lx, ly } = localCoords(e);

        //if element was already selected, test for clicks
        const sel = selectedEl();
        if (sel && hitResizeHandle(sel, lx, ly)) {
            drag = { mode: 'resize', el: sel, snap: { w: sel.w, h: sel.h, x: sel.x, y: sel.y }, startLX: lx, startLY: ly };
            return;
        }
        if (sel && sel.type === 'line') {
            const ep = hitLineEndpoint(sel, lx, ly);
            if (ep) { drag = { mode: ep, el: sel, startLX: lx, startLY: ly }; return; }
        }

        //if you click another element
        const hit = hitElement(lx, ly);
        if (hit) {
            selectedId = hit.id;           
            renderLayers(); 
            renderProps();
            if (hit.type === 'line') {
                drag = { mode: 'move-line', el: hit, startLX: lx, startLY: ly, snap: { x1: hit.x1, y1: hit.y1, x2: hit.x2, y2: hit.y2 } };
            } else {
                drag = { mode: 'move', el: hit, startLX: lx, startLY: ly, snap: { x: hit.x, y: hit.y } };
            }
        } else {
            selectedId = null;
            renderLayers();
            renderProps();
        }
        render();
    });

    window.addEventListener('mousemove', e => {
        if (!drag) return;
        const { lx, ly } = localCoords(e);
        const dx = Math.round(lx - drag.startLX);
        const dy = Math.round(ly - drag.startLY);
        const el = drag.el;

        if (drag.mode === 'move') {
            el.x = clamp(drag.snap.x + dx, 0, settings.screenW - 1);
            el.y = clamp(drag.snap.y + dy, 0, settings.screenH - 1);
        } else if (drag.mode === 'move-line') {
            el.x1 = clamp(drag.snap.x1 + dx, 0, settings.screenW - 1);
            el.y1 = clamp(drag.snap.y1 + dy, 0, settings.screenH - 1);
            el.x2 = clamp(drag.snap.x2 + dx, 0, settings.screenW - 1);
            el.y2 = clamp(drag.snap.y2 + dy, 0, settings.screenH - 1);
        } else if (drag.mode === 'resize') {
            el.w = clamp(drag.snap.w + dx, 2, settings.screenW - el.x);
            el.h = clamp(drag.snap.h + dy, 2, settings.screenH - el.y);
        } else if (drag.mode === 'p1') {
            el.x1 = clamp(Math.round(lx), 0, settings.screenW - 1);
            el.y1 = clamp(Math.round(ly), 0, settings.screenH - 1);
        } else if (drag.mode === 'p2') {
            el.x2 = clamp(Math.round(lx), 0, settings.screenW - 1);
            el.y2 = clamp(Math.round(ly), 0, settings.screenH - 1);
        }
        render(); generateCode(); refreshPropsValuesOnly();
    });

    window.addEventListener('mouseup', () => { drag = null; });

    // keyboard: delete + nudge
    window.addEventListener('keydown', e => {
        const tag = (document.activeElement && document.activeElement.tagName) || '';
        if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT') return;
        const el = selectedEl();
        if (!el) return;
        const step = e.shiftKey ? 8 : 1;
        let moved = false;
        if (e.key === 'Delete' || e.key === 'Backspace') {
            elements = elements.filter(x => x.id !== el.id);
            selectedId = null;
            renderLayers(); renderProps(); render(); generateCode();
            e.preventDefault();
            return;
        }
        const nudge = (dx, dy) => {
            if (el.type === 'line') { el.x1 += dx; el.y1 += dy; el.x2 += dx; el.y2 += dy; }
            else { el.x = clamp(el.x + dx, 0, settings.screenW - 1); el.y = clamp(el.y + dy, 0, settings.screenH - 1); }
        };
        if (e.key === 'ArrowLeft') { nudge(-step, 0); moved = true; }
        if (e.key === 'ArrowRight') { nudge(step, 0); moved = true; }
        if (e.key === 'ArrowUp') { nudge(0, -step); moved = true; }
        if (e.key === 'ArrowDown') { nudge(0, step); moved = true; }
        if (moved) { e.preventDefault(); render(); generateCode(); refreshPropsValuesOnly(); }
    });

    // ---------- layer panel ----------
    const layerList = document.getElementById('layerList');
    function renderLayers() {
        layerList.innerHTML = '';
        if (elements.length === 0) {
            const p = document.createElement('div');
            p.className = 'empty-hint';
            p.textContent = 'No elements yet! Add one above.';
            layerList.appendChild(p);
            return;
        }
        elements.forEach((el, i) => {
            const row = document.createElement('div');
            row.className = 'layer-row' + (el.id === selectedId ? ' selected' : '');
            row.innerHTML = `
        <span class="lbl">${escapeHtml(el.label)}</span>
        <button title="Move up" data-act="up" data-id="${el.id}">&uarr;</button>
        <button title="Move down" data-act="down" data-id="${el.id}">&darr;</button>
        <button title="Duplicate" data-act="dup" data-id="${el.id}">&#10064;</button>
        <button class="del" title="Delete" data-act="del" data-id="${el.id}">&times;</button>
      `;
            row.addEventListener('click', (e) => {
                if (e.target.tagName === 'BUTTON') return;
                selectedId = el.id; renderLayers(); renderProps(); render();
            });
            layerList.appendChild(row);
        });
    }
    layerList.addEventListener('click', e => {
        const btn = e.target.closest('button');
        if (!btn) return;
        const id = Number(btn.dataset.id);
        const idx = elements.findIndex(x => x.id === id);
        if (idx < 0) return;
        const act = btn.dataset.act;
        if (act === 'up' && idx < elements.length - 1) { [elements[idx], elements[idx + 1]] = [elements[idx + 1], elements[idx]]; }
        if (act === 'down' && idx > 0) { [elements[idx], elements[idx - 1]] = [elements[idx - 1], elements[idx]]; }
        if (act === 'del') { elements.splice(idx, 1); if (selectedId === id) selectedId = null; }
        if (act === 'dup') {
            const copy = JSON.parse(JSON.stringify(elements[idx]));
            copy.id = nextId++;
            copy.label = copy.label + ' copy';
            if ('x' in copy) copy.x = clamp(copy.x + 4, 0, settings.screenW - 1);
            if ('y' in copy) copy.y = clamp(copy.y + 4, 0, settings.screenH - 1);
            elements.splice(idx + 1, 0, copy);
            selectedId = copy.id;
        }
        renderLayers(); renderProps(); render(); generateCode();
    });

    // ---------- properties panel ----------
    const propsPanel = document.getElementById('propsPanel');

    function fieldNum(label, key, el, opts = {}) {
        return `<label class="field"><span class="lbltext">${label}</span>
      <input type="number" data-key="${key}" value="${el[key]}" ${opts.min !== undefined ? `min="${opts.min}"` : ''} ${opts.max !== undefined ? `max="${opts.max}"` : ''} step="${opts.step || 1}"></label>`;
    }
    function fieldText(label, key, el) {
        return `<label class="field"><span class="lbltext">${label}</span>
      <input type="text" data-key="${key}" value="${escapeAttr(el[key])}"></label>`;
    }

    function renderProps() {
        const el = selectedEl();
        if (!el) {
            propsPanel.innerHTML = renderGlobalSettings() + `<div class="placeholder-note">Select an element to edit its properties, or add a new one from the toolbar on the left.</div>`;
            bindGlobalSettings();
            return;
        }

        let body = `<h2>Selected: ${typeName[el.type]}</h2>`;
        body += fieldText('Label', 'label', el);

        if (el.type === 'rect' || el.type === 'rrect' || el.type === 'bitmap') {
            body += `<div class="row2">${fieldNum('X', 'x', el, { min: 0 })}${fieldNum('Y', 'y', el, { min: 0 })}</div>`;
            body += `<div class="row2">${fieldNum('W', 'w', el, { min: 2 })}${fieldNum('H', 'h', el, { min: 2 })}</div>`;
        }
        if (el.type === 'rrect') {
            body += fieldNum('Corner radius', 'r', el, { min: 0 });
        }
        if (el.type === 'rect' || el.type === 'rrect') {
            body += `<label class="chk"><input type="checkbox" data-key="filled" ${el.filled ? 'checked' : ''}> Filled</label>`;
        }
        if (el.type === 'line') {
            body += `<div class="row2">${fieldNum('X1', 'x1', el, { min: 0 })}${fieldNum('Y1', 'y1', el, { min: 0 })}</div>`;
            body += `<div class="row2">${fieldNum('X2', 'x2', el, { min: 0 })}${fieldNum('Y2', 'y2', el, { min: 0 })}</div>`;
        }
        if (el.type === 'text') {
            body += `<label class="field"><span class="lbltext">Text</span><textarea data-key="text">${escapeHtml(el.text)}</textarea></label>`;
            body += `<div class="row2">${fieldNum('X', 'x', el, { min: 0 })}${fieldNum('Y', 'y', el, { min: 0 })}</div>`;
            body += fieldNum('Text size', 'size', el, { min: 1, max: 6 });
            body += `<div class="placeholder-note" style="padding-top:0;">Width is estimated as 6px × size per character (Adafruit_GFX built-in font). Custom fonts will differ — treat as a guide.</div>`;
        }
        if (el.type === 'bitmap') {
            body += fieldText('Bitmap array name', 'name', el);
            body += `<div class="placeholder-note" style="padding-top:0;">Draws as a placeholder box in the preview. The generated code includes a zero-filled PROGMEM array you can replace with real bitmap data.</div>`;
        }

        body += `<label class="chk"><input type="checkbox" data-key="on" ${el.on ? 'checked' : ''}> "On" pixel color (uncheck for "off"/erase color)</label>`;

        body += renderGlobalSettings();

        propsPanel.innerHTML = body;

        propsPanel.querySelectorAll('[data-key]').forEach(inp => {
            inp.addEventListener('input', () => {
                const key = inp.dataset.key;
                if (inp.type === 'checkbox') el[key] = inp.checked;
                else if (inp.type === 'number') el[key] = Number(inp.value);
                else el[key] = inp.value;
                render(); renderLayers(); generateCode();
            });
        });
        bindGlobalSettings();
    }

    function refreshPropsValuesOnly() {
        // update number fields live during drag without full re-render (keeps focus/scroll stable)
        const el = selectedEl();
        if (!el) return;
        propsPanel.querySelectorAll('[data-key]').forEach(inp => {
            const key = inp.dataset.key;
            if (key in el && document.activeElement !== inp) {
                if (inp.type === 'checkbox') inp.checked = el[key];
                else inp.value = el[key];
            }
        });
    }

    function renderGlobalSettings() {
        return `
      <h2>Code output settings</h2>
      <label class="field"><span class="lbltext">Function name</span>
        <input type="text" id="s_funcName" value="${escapeAttr(settings.funcName)}"></label>
      <label class="field"><span class="lbltext">Graphics object name</span>
        <input type="text" id="s_gVar" value="${escapeAttr(settings.gVar)}"></label>
      <div class="row2">
        <label class="field"><span class="lbltext">"On" pixel constant</span><input type="text" id="s_onConst" value="${escapeAttr(settings.onConst)}"></label>
        <label class="field"><span class="lbltext">"Off" pixel constant</span><input type="text" id="s_offConst" value="${escapeAttr(settings.offConst)}"></label>
      </div>
      <label class="chk"><input type="checkbox" id="s_clear" ${settings.clearDisplay ? 'checked' : ''}> Include ${settings.gVar}.clearDisplay()</label>
      <label class="chk"><input type="checkbox" id="s_disp" ${settings.callDisplay ? 'checked' : ''}> Include ${settings.gVar}.display()</label>
      <div class="row2">
        <label class="field"><span class="lbltext">Screen width</span><input type="number" id="s_w" value="${settings.screenW}" min="8"></label>
        <label class="field"><span class="lbltext">Screen height</span><input type="number" id="s_h" value="${settings.screenH}" min="8"></label>
      </div>
    `;
    }
    function bindGlobalSettings() {
        const map = [
            ['s_funcName', 'funcName', 'text'], ['s_gVar', 'gVar', 'text'],
            ['s_onConst', 'onConst', 'text'], ['s_offConst', 'offConst', 'text'],
            ['s_clear', 'clearDisplay', 'checkbox'], ['s_disp', 'callDisplay', 'checkbox'],
            ['s_w', 'screenW', 'number'], ['s_h', 'screenH', 'number'],
        ];
        map.forEach(([id, key, type]) => {
            const inp = document.getElementById(id);
            if (!inp) return;
            inp.addEventListener('input', () => {
                if (type === 'checkbox') settings[key] = inp.checked;
                else if (type === 'number') settings[key] = Math.max(8, Number(inp.value) || settings[key]);
                else settings[key] = inp.value;
                if (id === 's_clear' || id === 's_disp') { /* label text has gVar baked in, refresh */ }
                render(); generateCode();
                if (id === 's_clear' || id === 's_disp' || id === 's_gVar') { renderProps(); }
            });
        });
    }

    // ---------- code generation ----------
    function escapeCpp(s) { return s.replace(/\\/g, '\\\\').replace(/"/g, '\\"'); }
    function escapeHtml(s) { return String(s).replace(/[&<>]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;' }[c])); }
    function escapeAttr(s) { return String(s).replace(/"/g, '&quot;'); }

    function highlight(code) {
        const esc = escapeHtml(code);
        const lines = esc.split('\n').map(line => {
            if (/^\s*\/\//.test(line)) return `<span class="cm">${line}</span>`;
            let l = line.replace(/"(.*?)"/g, m => `<span class="st">${m}</span>`);
            l = l.replace(/\b(void|static|const|unsigned|char)\b/g, '<span class="kw">$1</span>');
            return l;
        });
        return lines.join('\n');
    }

    function generateCode() {
        const s = settings;
        const lines = [];
        lines.push(`void ${s.funcName}() {`);
        if (s.clearDisplay) lines.push(`  ${s.gVar}.clearDisplay();`);
        if (elements.length === 0) {
            lines.push(`  // No elements yet -- add some from the toolbar!`);
        }
        elements.forEach(el => {
            const c = el.on ? s.onConst : s.offConst;
            lines.push('');
            lines.push(`  // ${el.label}`);
            if (el.type === 'rect') {
                lines.push(`  ${s.gVar}.${el.filled ? 'fillRect' : 'drawRect'}(${el.x}, ${el.y}, ${el.w}, ${el.h}, ${c});`);
            } else if (el.type === 'rrect') {
                lines.push(`  ${s.gVar}.${el.filled ? 'fillRoundRect' : 'drawRoundRect'}(${el.x}, ${el.y}, ${el.w}, ${el.h}, ${el.r}, ${c});`);
            } else if (el.type === 'line') {
                lines.push(`  ${s.gVar}.drawLine(${el.x1}, ${el.y1}, ${el.x2}, ${el.y2}, ${c});`);
            } else if (el.type === 'text') {
                lines.push(`  ${s.gVar}.setTextColor(${c});`);
                lines.push(`  ${s.gVar}.setTextSize(${el.size});`);
                lines.push(`  ${s.gVar}.setCursor(${el.x}, ${el.y});`);
                lines.push(`  ${s.gVar}.print("${escapeCpp(el.text)}");`);
            } else if (el.type === 'bitmap') {
                lines.push(`  ${s.gVar}.drawBitmap(${el.x}, ${el.y}, ${el.name}, ${el.w}, ${el.h}, ${c});`);
            }
        });
        if (s.callDisplay) { lines.push(''); lines.push(`  ${s.gVar}.display();`); }
        lines.push(`}`);

        let out = '';
        const bitmaps = elements.filter(e => e.type === 'bitmap');
        if (bitmaps.length) {
            out += `// Bitmap data -- declare above the function, replace zeros with real image bytes.\n`;
            out += `// Each row is packed MSB-first, ${'${bytesPerRow}'.length ? '' : ''}rounded up to a whole byte per row.\n\n`;
            bitmaps.forEach(b => {
                const bytesPerRow = Math.ceil(b.w / 8);
                const total = bytesPerRow * b.h;
                out += `static const unsigned char PROGMEM ${b.name}[] = {\n`;
                out += `  // ${b.w}x${b.h}px, ${bytesPerRow} byte(s)/row, ${total} bytes total\n`;
                const rowBytes = new Array(bytesPerRow).fill('0x00').join(', ');
                for (let r = 0; r < b.h; r++) out += `  ${rowBytes},\n`;
                out = out.replace(/,\n$/, '\n');
                out += `};\n\n`;
            });
        }
        out += lines.join('\n');

        document.getElementById('codeOut').innerHTML = highlight(out);
    }

    document.getElementById('copyBtn').addEventListener('click', () => {
        const text = document.getElementById('codeOut').textContent;
        const btn = document.getElementById('copyBtn');
        const done = () => { const old = btn.textContent; btn.textContent = 'Copied!'; setTimeout(() => btn.textContent = old, 1100); };
        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(text).then(done).catch(() => fallbackCopy(text, done));
        } else fallbackCopy(text, done);
    });
    function fallbackCopy(text, done) {
        const ta = document.createElement('textarea');
        ta.value = text; document.body.appendChild(ta); ta.select();
        try { document.execCommand('copy'); done(); } catch (e) { }
        document.body.removeChild(ta);
    }

    // ---------- toolbar wiring ----------
    document.querySelectorAll('[data-add]').forEach(btn => {
        btn.addEventListener('click', () => {
            const el = makeElement(btn.dataset.add);
            elements.push(el);
            selectedId = el.id;
            renderLayers(); renderProps(); render(); generateCode();
        });
    });

    document.getElementById('zoom_slider').addEventListener('input', e => {
        settings.zoom = Number(e.target.value); render();
    });
    document.getElementById('gridChk').addEventListener('change', e => {
        settings.showGrid = e.target.checked; 
        render();
    });
    document.getElementById('clearBtn').addEventListener('click', () => {
        if (elements.length && !confirm('Clear all elements?')) return;
        elements = []; selectedId = null;
        renderLayers(); renderProps(); render(); generateCode();
    });

    // ---------- init ----------
    renderLayers();
    renderProps();
    render();
    generateCode();

})();