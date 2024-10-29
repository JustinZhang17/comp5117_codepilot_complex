document.addEventListener("DOMContentLoaded", () => {
  const examples = document.getElementById("examples");
  let exampleCount = 2;

  const addExample = () => {
    const exampleEle = ` 
      <div id="example-block-${exampleCount}" class="w-full border-2 p-6 rounded-lg my-3">
        <label for="example-code-${exampleCount}" class="block text-sm font-medium text-gray-700 mt-4">
          Example code snippet ${exampleCount}
        </label>

        <textarea
          id="example-code-${exampleCount}"
          class="mt-2 w-full rounded-lg border-gray-200 p-4 align-top shadow-md border-2 sm:text-sm outline-none"
          rows="10"
          placeholder="Enter your example code here"
        ></textarea>

        <label for="example-plan-${exampleCount}" class="block text-sm font-medium text-gray-700 mt-4">
          Example plan ${exampleCount}
        </label>

        <textarea
          id="example-plan-${exampleCount}"
          class="mt-2 w-full rounded-lg border-gray-200 p-4 align-top shadow-md border-2 sm:text-sm outline-none"
          rows="10"
          placeholder="Enter your plan here"
        ></textarea>

        <label for="example-coverage-${exampleCount}" class="block text-sm font-medium text-gray-700 mt-4">
          Example Code Coverage ${exampleCount}
        </label>

        <textarea
          id="example-coverage-${exampleCount}"
          class="mt-2 w-full rounded-lg border-gray-200 p-4 align-top shadow-md border-2 sm:text-sm outline-none"
          rows="10"
          placeholder="Enter your example here"
        ></textarea>
      </div>
    `;
    exampleCount++;
    examples.insertAdjacentHTML("beforeend", exampleEle);
  };

  const removeExample = () => {
    if (exampleCount < 2) return;
    const child = document.getElementById(`example-block-${exampleCount - 1}`);
    examples.removeChild(child);
    exampleCount--;
  };

  const submitCodePilot = async () => {
    const button = document.getElementById("submit");
    button.disabled = true;
    button.textContent = "Loading ...";

    const cpObj = {};
    const examples = [];

    const model = document.getElementById("model");
    if (!model.value) {
      button.disabled = false;
      button.textContent = "Submit";
      return;
    }
    cpObj["model"] = model.value;

    const phase = document.getElementById("phase");
    if (!phase.value) {
      button.disabled = false;
      button.textContent = "Submit";
      return;
    }
    cpObj["phase"] = phase.value;

    for (let i = 1; i < exampleCount; i++) {
      const code = document.getElementById(`example-code-${i}`).value;
      const plan = document.getElementById(`example-plan-${i}`).value;
      const cov = document.getElementById(`example-coverage-${i}`).value;

      if (!code || !plan || !cov) {
        button.disabled = false;
        button.textContent = "Submit";
        return;
      }
      const obj = {
        code: code,
        plan: plan,
        coverage: cov,
      };

      examples.push(obj);
    }

    cpObj["examples"] = examples;

    const snip = document.getElementById("code-snippet");
    if (!snip.value) {
      button.disabled = false;
      button.textContent = "Submit";
      return;
    }
    cpObj["code"] = snip.value;

    const api = document.getElementById("api-key");
    if (!api.value) {
      button.disabled = false;
      button.textContent = "Submit";
      return;
    }

    cpObj["api-key"] = api.value;

    try {
      await fetch("/" + cpObj["phase"], {
        method: "POST",
        body: JSON.stringify(cpObj),
        headers: {
          "Content-Type": "application/json",
        },
      })
        .then((res) => res.blob())
        .then((blob) => {
          const file = window.URL.createObjectURL(blob);
          var a = document.createElement("a");
          a.href = file;
          a.download = "output.txt";
          document.body.appendChild(a);
          a.click();
          a.remove();
          button.disabled = false;
          button.textContent = "Submit";
        });
    } catch (error) {
      console.error(error.message);
    }
  };

  const addExampleButton = document.getElementById("addExample");
  addExampleButton.addEventListener("click", addExample);

  const removeExampleButton = document.getElementById("removeExample");
  removeExampleButton.addEventListener("click", removeExample);

  const submit = document.getElementById("submit");
  submit.addEventListener("click", submitCodePilot);
});
