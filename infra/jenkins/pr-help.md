The following is a list of valid commands on this PR.
To invoke test, comment the indicated phrase

- Base command

<details>
  <summary>Click to expand</summary>

Comment Phrase | Action
-------------- | ------
@nnfw-bot test all | Re-run all default legs.  Use sparingly
@nnfw-bot help | Print this help message

</details>

- Command for each test legs

<details>
  <summary>Click to expand</summary>

Comment Phrase | Default | Action
-------------- | ------- | ------
@nnfw-bot test format-checker | ON | Re-run format-checker
@nnfw-bot test nobody-checker | ON | Re-run nobody-checker
@nnfw-bot test signed-off-by-checker| ON | Re-run signed-off-by-checker
@nnfw-bot test copyright-checker | ON | Re-run copyright-checker
@nnfw-bot test nncc-debug | ON | Re-run nncc-debug
@nnfw-bot test cross-runtime-test-debug | ON | Re-run cross-runtime-test-debug
@nnfw-bot test cross-runtime-test-release | OFF | Re-run cross-runtime-test-release
@nnfw-bot test cross-aarch64-runtime-test-debug | ON | Re-run cross-runtime-test-debug
@nnfw-bot test cross-aarch64-runtime-test-release | OFF | Re-run cross-runtime-test-release
@nnfw-bot test x64-builder | ON | Re-run x64-builder
@nnfw-bot test tizen-gbs-builder | OFF | Re-run tizen-gbs-builder
@nnfw-bot test nnpackage | OFF | Re-run nnpackage-test

</details>
