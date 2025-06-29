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

    // Handle delete form submission
    document.getElementById("delete-form").addEventListener("submit", async function (e) {
        e.preventDefault();
        const path = document.getElementById("delete-path").value;
        
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