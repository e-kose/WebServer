// GET form submission handler
document.getElementById("get-form").addEventListener("submit", function (e) {
    e.preventDefault();
    const path = document.getElementById("get-path").value;
    
    if (!path.trim()) {
        alert("❌ Lütfen bir yol girin!");
        return;
    }
    window.location.href = path;
});
document.getElementById("upload-form").addEventListener("submit", async function (e) {
        e.preventDefault();
        
        const formData = new FormData(this);
        const fileInput = document.getElementById("file-input");
        
        if (!fileInput.files.length) {
            alert("❌ Lütfen bir dosya seçin!");
            return;
        }

        try {
            const response = await fetch(this.action, {
                method: 'POST',
                body: formData
            });

            if (response.ok) {
                alert("✅ Dosya Başarıyla Yüklendi!");
            } else {
                alert(`❌ Yükleme Başarısız (${response.status})`);
            }
        } catch (err) {
            alert("❌ Ağ Hatası: Dosya gönderilemedi");
        }
    });

document.getElementById("delete-form").addEventListener("submit", async function (e) {
    e.preventDefault();
    const path = document.getElementById("delete-path").value;
    
    // Dosya yolu kontrolü
    if (!path.trim()) {
        alert("❌ Lütfen silinecek dosya yolunu girin!");
        return;
    }
    
    // Ek güvenlik kontrolü - boş veya sadece / olan yolları engelle
    if (path.trim() === "/" || path.trim() === "") {
        alert("❌ Geçersiz dosya yolu!");
        return;
    }
    
    try {
        const response = await fetch(path, { method: "DELETE" });
        if (response.ok) {
            alert("✅ Dosya Silindi");
        } else {
            alert(`❌ Silme İşlemi Başarısız (${response.status})`);
        }
    } catch (err) {
        alert("❌ Ağ Hatası: Silme işlemi yapılamadı");
    }
});
document.getElementById("return-form").addEventListener("submit", function (e) {
    e.preventDefault();
    const path = document.getElementById("return-path").value;
    
    if (!path.trim()) {
        alert("❌ Lütfen bir yol girin!");
        return;
    }
    
    // Return/Redirect isteği için sayfayı yönlendir
    window.location.href = path;
});