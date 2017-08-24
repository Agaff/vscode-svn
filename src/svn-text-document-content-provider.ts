import * as vscode from "vscode";

import { SvnClient, SvnError } from "./svn-client";

export class SvnTextDocumentContentProvider implements vscode.TextDocumentContentProvider, vscode.Disposable {
    private onDidChangeEvent: vscode.EventEmitter<vscode.Uri> = new vscode.EventEmitter<vscode.Uri>();

    constructor(private client: SvnClient) { }

    get onDidChange() { return this.onDidChangeEvent.event; }

    onCommit(states: vscode.SourceControlResourceState[]) {
        for (const value of states)
            this.onDidChangeEvent.fire(value.resourceUri.with({ scheme: "svn" }));
    }

    async provideTextDocumentContent(uri: vscode.Uri, token: vscode.CancellationToken): Promise<string> {
        try {
            return await this.client.cat(uri.fsPath);
        } catch (err) {
            if (err instanceof SvnError)
                return "";
            throw err;
        }
    }

    dispose() {
        this.onDidChangeEvent.dispose();
    }
}